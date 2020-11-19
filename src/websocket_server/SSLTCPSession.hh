#ifndef WEBSOCKET_SERVER_SSL_TCP_SESSION_HH
#define WEBSOCKET_SERVER_SSL_TCP_SESSION_HH

#include "websocket_server/TCPSession.hh"

#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>

/// TODO: Inherit from 'BasicSSLSession' which already implements functions like
/// async ssl handshake, and async ssl shutdown. The derived class will simply
/// call the base class. The design will be static polymorphism, aka. CRTP.

namespace amadeus {
class SSLTCPSession
    : public TCPSession<SSLTCPSession>
    , public std::enable_shared_from_this<SSLTCPSession>
{
  private:
    /// The underlying TCP SSL stream.
    beast::ssl_stream<beast::tcp_stream> stream_;

    char data_[1024];

    /// \brief CompletionToken for the asynchronous SSL handshake.
    void onHandshake(beast::error_code const& _error,
                     std::size_t _bytesTransferred);

    /// \brief CompletionToken for the asynchronous SSL shutdown.
    void onShutdown(beast::error_code const& _error);

    void onWrite(beast::error_code const& _error,
                 std::size_t _bytesTransferred);

    void doWrite(std::size_t _bytes);

  public:
    /// \brief Create a secure TCP Session.
    SSLTCPSession(beast::tcp_stream&& _stream, ssl::context& _ctx,
                  beast::flat_buffer&& _buffer,
                  std::shared_ptr<SharedState> const& _state);

    ~SSLTCPSession();

    /// \brief Return the underlying TCP SSL stream.
    beast::ssl_stream<beast::tcp_stream>& stream() noexcept;

    /// \brief Returns and moves ownership of the underlying TCP SSL Stream to
    /// the caller.
    beast::ssl_stream<beast::tcp_stream> releaseStream() noexcept;

    /// \brief Start the TCP Session.
    void run();

    /// \brief Ends the SSL TCP Session.
    /// \note Called by the base class.
    void disconnect();
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_TCP_SESSION_HH