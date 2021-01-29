#ifndef WEBSOCKET_SERVER_TCP_SESSION_HH
#define WEBSOCKET_SERVER_TCP_SESSION_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/TCPRequestHandler.hh"
#include "websocket_server/SharedState.hh"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/stream_traits.hpp>

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

    /// An alias for the TCPRequestHandler tied to the current TCPSession.
    using PacketHandlerType = TCPRequestHandler<class TCPSession>;

    /// The shared state.
    std::shared_ptr<SharedState> state_;
    /// The underlying input buffer for TCP responses.
    std::array<char, MaxInputSize> input_{};
    /// The underlying output buffer for TCP requests.
    std::array<char, MaxOutputSize> output_{};
    /// The number of bytes we have left to read.
    std::size_t numBytesLeft_{};
    /// The underlying TCPRequestHandler for the µc-connection.
    TCPRequestHandler<TCPSession> handler_;
    /// Each session is uniquely identified with the StationId.
    StationId stationId_;

    /// \brief CompletionToken for the asynchronous read operation.
    /// \param _error The error.
    /// \param _bytesTransferred The number of bytes transferred during the
    /// asynchronous TCP read.
    void onReadPacketHeader(beast::error_code const& _error,
                            std::size_t _bytesTransferred)
    {
        if (_error) {
            handler_.stop();
            derived().disconnect();
            return;
        }

        if (_error == asio::error::eof ||
            _error == asio::error::connection_reset) {
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
        // As long as we have packets in the buffer pending, we should parse
        // them all until there are no more bytes to be read from the buffer.
        while (numBytesLeft_) {
            // Extract the PacketId from the buffer.
            auto const id = static_cast<in::PacketType>(*input_.begin());

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

            switch (status) {
            case ResultType::Good: {
                // do we still need to parse?
                if (numBytesLeft_ > 0) {
                    continue;
                }
                // we are done, read another request.
                return doReadPacketHeader();
            } break;
            case ResultType::Bad: {
                // return derived().disconnect();
                return;
            } break;
            case ResultType::Indeterminate: {
                auto const packetName = in::packetNameById(id);
                LOG_DEBUG("Incomplete Packet received ({})--> Reading more.\n",
                          packetName);

                beast::get_lowest_layer(derived().stream())
                    .expires_after(std::chrono::seconds(Timeout));

                return derived().stream().async_read_some(
                    asio::buffer(input_.data() + numBytesLeft_,
                                 input_.size() - numBytesLeft_),
                    [self = derived().shared_from_this()](
                        auto&& error, auto&& bytes_transferred) {
                        self->onReadPacketHeader(error, bytes_transferred);
                    });
            } break;
            }
        }
    }

  public:
    /// \brief Creates a TCPSession.
    /// \param _ioc A reference to the io_context.
    /// \param _state The SharedState.
    TCPSession(asio::io_context& _ioc, std::shared_ptr<SharedState> _state)
        : state_(std::move(_state))
        , handler_(_ioc, *this)
    {
    }

    /// \brief Leaves the TCPSession.
    ~TCPSession()
    {
        state_->leave<Derived>(stationId_);
        LOG_DEBUG("TCPSession disconnected.\n");
    }

    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief Returns the underlying SharedState reference.
    SharedState const& sharedState() const noexcept
    {
        return *state_.get();
    };

    /// \brief Returns the underlying SharedState reference.
    SharedState& sharedState() noexcept
    {
        return *state_.get();
    };

    /// \brief Returns the stationId for the current TCP Session.
    StationId const stationId() const noexcept
    {
        return stationId_;
    }

    /// \brief Sets the stationId for the current TCP Session.
    void stationId(StationId _id) noexcept
    {
        stationId_ = _id;
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
    /// \tparam Packet The Packet to be written to the TCP stream.
    /// \tparam CompletionHandler The function object to inform the caller about
    /// the asynchronous operation which is immediately called once the given
    /// packet was transferred to the remote endpoint.
    /// \param _packet A const reference to the given packet.
    /// \param _handler Any valid function object with the function signature
    /// described above.
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

    /// \brief Starts the asynchronous communication by sending a 'Handshake'
    /// packet.
    void run()
    {
        out::HandshakePacket packet{};

        writePacket(packet, [this](auto&& bytes_transferred) {
            LOG_INFO("HandshakePacket sent with {} bytes.\n",
                     bytes_transferred);

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
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_TCP_SESSION_HH
