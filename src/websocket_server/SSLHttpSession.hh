#ifndef WEBSOCKET_SERVER_SSL_HTTP_SESSION_HH
#define WEBSOCKET_SERVER_SSL_HTTP_SESSION_HH

#include "websocket_server/HttpSession.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

/// TODO: Inherit from 'BasicSSLSession' which already implements functions like
/// async ssl handshake, and async ssl shutdown. The derived class will simply
/// call the base class. The design will be static polymorphism, aka. CRTP.

namespace amadeus {
class SSLHttpSession
    : public HttpSession<SSLHttpSession>
    , public std::enable_shared_from_this<SSLHttpSession>
{
  private:
    /// The underlying SSL TCP stream.
    beast::ssl_stream<beast::tcp_stream> stream_;

    /// \brief CompletionToken for the asynchronous SSL handshake.
    void onHandshake(beast::error_code const& _error,
                     std::size_t _bytesTransferred);

    /// \brief CompletionToken for the asynchronous SSL shutdown.
    void onShutdown(beast::error_code const& _error);

  public:
    SSLHttpSession(beast::tcp_stream&& _stream, ssl::context& _ctx,
                   beast::flat_buffer&& _buffer,
                   std::shared_ptr<SharedState> const& _state);

    ~SSLHttpSession();

    /// \brief Return the underlying TCP stream.
    beast::ssl_stream<beast::tcp_stream>& stream() noexcept
    {
        return stream_;
    }

    /// \brief Returns and moves ownership of the underlying TCP stream to the
    /// caller.
    beast::ssl_stream<beast::tcp_stream> releaseStream() noexcept
    {
        return std::move(stream_);
    }

    /// \brief Starts the SSL HTTP Session.
    void run();

    /// \brief Ends the SSL HTTP Session.
    /// \note Called by the base class.
    void disconnect();
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_HTTP_SESSION_HH