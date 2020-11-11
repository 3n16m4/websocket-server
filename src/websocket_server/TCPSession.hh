#ifndef WEBSOCKET_SERVER_TCP_SESSION_HH
#define WEBSOCKET_SERVER_TCP_SESSION_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/SharedState.hh"
#include "websocket_server/Common.hh"

#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <array>
#include <memory>
#include <stdexcept>
#include <iostream>
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

    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief CompletionToken for the asynchronous read operation.
    void onRead(beast::error_code const& _error, std::size_t _bytesTransferred)
    {
        if (_error || _bytesTransferred == 0) {
            std::cerr << "Error: " << _error.message() << '\n';
            return;
        }

        beast::get_lowest_layer(derived().stream())
            .expires_after(std::chrono::seconds(Timeout));

        std::cout << _error.message() << " " << _bytesTransferred << '\n';
        std::cout << "Message " << beast::buffers_to_string(buffer_.data()) << '\n';
        
        asio::async_write(derived().stream(), buffer_,
                          [self = derived().shared_from_this()](
                              auto&& ec, auto&& bytes_transferred) {
                              self->onWrite(ec, bytes_transferred);
                          });
    }

    /// \brief CompletionToken for the asynchronous write operation.
    void onWrite(beast::error_code const& _error, std::size_t _bytesTransferred)
    {
        std::cout << "Sent " << beast::buffers_to_string(buffer_.data()) << " "
                  << _bytesTransferred << " bytes.\n";

        // read another packet.
        asio::async_read(derived().stream(), buffer_,
                         [self = derived().shared_from_this()](
                             auto&& ec, auto&& bytes_transferred) {
                             self->onRead(ec, bytes_transferred);
                         });
    }

  public:
    /// \brief Constructor.
    TCPSession(beast::flat_buffer&& _buffer,
               std::shared_ptr<SharedState> const& _state)
        : buffer_(std::move(_buffer))
        , state_(_state)
    {
    }

    /// \brief Starts the asynchronous read operation.
    void doRead()
    {
        std::cout << "TCPSession::doRead()\n";

        beast::get_lowest_layer(derived().stream())
            .expires_after(std::chrono::seconds(Timeout));

        asio::async_write(derived().stream(), buffer_,
                          [self = derived().shared_from_this()](
                              auto&& ec, auto&& bytes_transferred) {
                              self->onWrite(ec, bytes_transferred);
                          });
    }

    /// \brief Starts the asynchronous write operation.
    void doWrite()
    {
        throw std::runtime_error("NotImplemented");
    }

  protected:
    /// The underlying buffer for incoming TCP requests.
    // std::array<char, 1024> buffer_;
    beast::flat_buffer buffer_;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_TCP_SESSION_HH