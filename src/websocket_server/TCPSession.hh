#ifndef WEBSOCKET_SERVER_TCP_SESSION_HH
#define WEBSOCKET_SERVER_TCP_SESSION_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/SharedState.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/PacketHandler.hh"
#include "websocket_server/Packets/Out/HandshakePacket.hh"

#include <boost/beast/core/error.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <array>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace amadeus {
/// CRTP is used here to avoid code duplication and virtual function calls.
/// This is not only beneficial for performance but also to allow SSL
/// TCPSessions and regular TCPSessions to work with the same code.
/// \brief Provides a simple default TCPSession implementation.
template <class Derived>
class TCPSession
{
  private:
    /// The maximum amount of bytes for the input buffer.
    static constexpr auto MaxInputSize{512U};
    /// The maximum amount of bytes for the output buffer.
    static constexpr auto MaxOutputSize{512U};

    /// The shared state.
    std::shared_ptr<SharedState> state_;
    /// The underlying input buffer for TCP responses.
    std::array<char, MaxInputSize> input_{};
    /// The underlying output buffer for TCP requests.
    std::array<char, MaxOutputSize> output_{};
    /// The number of bytes we have left to read.
    std::size_t numBytesLeft_{};
    /// The underlying PacketHandler for the µc-connection.
    PacketHandler handler_;

    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief CompletionToken for the asynchronous read operation.
    void onReadPacketHeader(beast::error_code const& _error,
                            std::size_t _bytesTransferred)
    {
        if (_error == asio::error::eof ||
            _error == asio::error::connection_reset) {
            /// TODO: onDisconnected
            LOG_ERROR("Read error: Connection was closed by remote endpoint\n");
            return;
        }
        if (_error == asio::error::operation_aborted ||
            _error == asio::error::connection_aborted) {
            LOG_ERROR("Read error: Connection was closed by remote endpoint "
                      "due to a timeout.\n");
            return;
        }
        if (_error) {
            LOG_ERROR("Read error: {}\n", _error.message());
            return;
        }

        // Disable the timeout for the next logical operation.
        beast::get_lowest_layer(derived().stream()).expires_never();

        // accumulate the bytes we have read.
        numBytesLeft_ += _bytesTransferred;

        // parse the data that was read.
        parsePacketHeader();
    }

    /// \brief Parses the received packet from the input buffer.
    void parsePacketHeader()
    {
        // Extract the PacketId from the buffer.
        auto const& id = *input_.begin();

        // As long as we have packets in the buffer pending, we should parse
        // them all until there are no more bytes to be read from the buffer.
        while (numBytesLeft_) {
            // Get a view of the data we have received.
            auto const view{asio::const_buffer(input_.data(), numBytesLeft_)};

            // Parse the packet header by id.
            auto const [status, bytesParsed] = handler_.handle(id, view);

            // Pop off the current packet we read. (Comparable to the erase
            // member function for std::vector.)
            std::memmove(input_.data(), input_.data() + bytesParsed,
                         numBytesLeft_ - bytesParsed);

            // Update the number of bytes we have left to read.
            numBytesLeft_ -= bytesParsed;

            using ResultType = PacketHandler::ResultType;
            switch (status) {
            case ResultType::Good:
                break;
            case ResultType::Bad:
                break;
            case ResultType::Indeterminate:
                break;
            }
        }
    }

    /// \brief CompletionToken for the asynchronous write operation.
    void onWrite(beast::error_code const& _error, std::size_t _bytesTransferred)
    {
        std::string_view const message{input_.begin(), _bytesTransferred};
        LOG_DEBUG("Sent {} {} bytes\n", message.data(), message.size());

        // read another packet.
        doReadPacketHeader();
    }

    // clang-format off
    /// \brief Send a packet and be notified about the asynchronous write
    /// operation through the CompletionHandler.
    /// CompletionHandler must have the following signature:
    /// void on_packet_sent(std::size_t bytes_transferred)
    /// {
    ///     // ...
    /// }
    // clang-format on
    /// TODO: perhaps the completionhandler is not even important because we are
    /// only using this function in this class anyway.
    template <typename Packet, typename CompletionHandler>
    void writePacket(Packet const& _packet, CompletionHandler&& _handler)
    {
        auto constexpr BodySize = sizeof(Packet);

        static_assert(BodySize <= MaxOutputSize,
                      "Packet size exceeds output buffer.");
        static_assert(std::is_class<Packet>::value,
                      "Packet needs to be a struct.");
        static_assert(std::is_trivially_copyable<Packet>::value,
                      "Packet needs to be trivially copyable.");

        std::memcpy(output_.data(), &_packet, BodySize);

        asio::async_write(derived().stream(), asio::buffer(output_, BodySize),
                          [self = derived().shared_from_this(),
                           _handler = std::move(_handler)](
                              auto&& error, auto&& bytes_transferred) {
                              if (error) {
                                  LOG_ERROR("Write error: {}\n",
                                            error.message());
                                  return;
                              }
                              _handler(bytes_transferred);
                          });
    }

  public:
    /// \brief Constructor.
    TCPSession(std::shared_ptr<SharedState> const& _state)
        : state_(_state)
    {
    }

    /// \brief Starts the asynchronous communication by sending a 'Handshake'
    /// packet.
    void run()
    {
        HandshakePacket packet{};

        writePacket(packet, [this](auto&& bytes_transferred) {
            LOG_INFO("Handshake packet sent with {} bytes.\n",
                     bytes_transferred);

            /// TODO: Start reading packets...
            doReadPacketHeader();
        });
    }

    /// \brief Starts the asynchronous read operation.
    void doReadPacketHeader()
    {
        LOG_DEBUG("TCPSession::doReadPacketHeader()\n");

        beast::get_lowest_layer(derived().stream())
            .expires_after(std::chrono::seconds(Timeout));

        derived().stream().async_read_some(
            asio::buffer(input_), [self = derived().shared_from_this()](
                                      auto&& ec, auto&& bytes_transferred) {
                self->onReadPacketHeader(ec, bytes_transferred);
            });
    }

    /// \brief Starts the asynchronous write operation.
    void doWrite()
    {
        throw std::runtime_error("NotImplemented");
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_TCP_SESSION_HH