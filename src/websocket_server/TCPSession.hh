#ifndef WEBSOCKET_SERVER_TCP_SESSION_HH
#define WEBSOCKET_SERVER_TCP_SESSION_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/SharedState.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/Logger.hh"

#include <boost/beast/core/error.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <array>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace amadeus {
/// CRTP is used here to avoid code duplication and virtual function calls.
/// This is not only beneficial for performance but also to allow SSL
/// TCPSessions and regular TCPSessions to work with the same code.
/// \brief Provides a simple default TCPSession implementation.
template <class Derived>
class TCPSession
{
  private:
    /// The shared state.
    std::shared_ptr<SharedState> state_;
    /// The underlying buffer for incoming TCP requests.
    std::array<char, 1024> buffer_;

    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief CompletionToken for the asynchronous read operation.
    void onRead(beast::error_code const& _error, std::size_t _bytesTransferred)
    {
        if (_error) {
            LOG_ERROR("Read error: {}\n", _error.message());
            return;
        }

        beast::get_lowest_layer(derived().stream())
            .expires_after(std::chrono::seconds(Timeout));

        asio::async_write(derived().stream(),
                          asio::buffer(buffer_.data(), _bytesTransferred),
                          [self = derived().shared_from_this()](
                              auto&& ec, auto&& bytes_transferred) {
                              self->onWrite(ec, bytes_transferred);
                          });

        /// TODO: Parse the packet (id, body)
        /// pass the id and body to the PacketHandler.
    }

    /// \brief CompletionToken for the asynchronous write operation.
    void onWrite(beast::error_code const& _error, std::size_t _bytesTransferred)
    {
        std::string_view const message{buffer_.data(), _bytesTransferred};
        LOG_DEBUG("Sent {} {} bytes\n", message.data(), message.size());

        // read another packet.
        doRead();
    }

  public:
    /// \brief Constructor.
    TCPSession(std::shared_ptr<SharedState> const& _state)
        : state_(_state)
    {
    }

    /// \brief Starts the asynchronous read operation.
    void doRead()
    {
        LOG_DEBUG("TCPSession::doRead()\n");

        beast::get_lowest_layer(derived().stream())
            .expires_after(std::chrono::seconds(Timeout));

        derived().stream().async_read_some(
            asio::buffer(buffer_), [self = derived().shared_from_this()](
                         auto&& ec, auto&& bytes_transferred) {
                self->onRead(ec, bytes_transferred);
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