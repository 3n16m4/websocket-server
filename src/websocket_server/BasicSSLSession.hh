#ifndef WEBSOCKET_SERVER_BASIC_SSL_SESSION_HH
#define WEBSOCKET_SERVER_BASIC_SSL_SESSION_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/Logger.hh"

#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/flat_buffer.hpp>

#include <stdexcept>

namespace amadeus {
template <class Derived>
class BasicSSLSession
{
  private:
    /// The underlying TCP SSL stream.
    beast::ssl_stream<beast::tcp_stream> stream_;
    /// The underlying buffer for handshake.
    beast::flat_buffer buffer_;

    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief CompletionToken for the asynchronous SSL handshake.
    void onHandshake(beast::error_code const& _error,
                     std::size_t _bytesTransferred)
    {
        if (_error) {
            LOG_ERROR("Handshake error: {}\n", _error.message());
            return;
        }

        buffer_.consume(_bytesTransferred);

        derived().onHandshake(_error);
    }

    /// \brief CompletionToken for the asynchronous SSL shutdown.
    void onShutdown(beast::error_code const& _error)
    {
        throw std::runtime_error("NotImplemented");

        // derived().onShutdown(_error);
    }

  public:
    /// \brief Constructor.
    explicit BasicSSLSession(beast::tcp_stream&& _stream, ssl::context& _ctx,
                             beast::flat_buffer _buffer)
        : stream_(std::move(_stream), _ctx)
        , buffer_(_buffer)
    {
    }

    /// \brief Return the underlying TCP SSL stream.
    beast::ssl_stream<beast::tcp_stream>& stream() noexcept
    {
        return stream_;
    }

    /// \brief Returns and moves ownership of the underlying TCP SSL Stream to
    /// the caller.
    beast::ssl_stream<beast::tcp_stream> releaseStream() noexcept
    {
        return std::move(stream_);
    }

    /// \brief Starts the async SSL handshake.
    void run()
    {
        beast::get_lowest_layer(stream_).expires_after(Timeout);

        // Perform the async SSL handshake.
        stream_.async_handshake(ssl::stream_base::server, buffer_.data(),
                                [this](auto&& ec, auto&& bytes_transferred) {
                                    onHandshake(ec, bytes_transferred);
                                });
    }

    /// \brief Ends the SSL TCP Session.
    void disconnect()
    {
        beast::get_lowest_layer(stream_).expires_after(Timeout);

        // Perform the async SSL shutdown.
        stream_.async_shutdown([self = derived().shared_from_this()](
                                   auto&& ec) { self->onShutdown(ec); });
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_BASIC_SSL_SESSION_HH