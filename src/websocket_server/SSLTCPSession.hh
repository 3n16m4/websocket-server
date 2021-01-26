#ifndef WEBSOCKET_SERVER_SSL_TCP_SESSION_HH
#define WEBSOCKET_SERVER_SSL_TCP_SESSION_HH

#include "websocket_server/TCPSession.hh"

#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>

namespace amadeus {
/// \brief Describes a simple SSLTCPSession which derives from the CRTP
/// TCPSession class. Represents a single SSL TCP Session on the network which
/// describes a µC connection.
class SSLTCPSession
    : public TCPSession<SSLTCPSession>
    , public std::enable_shared_from_this<SSLTCPSession>
{
  private:
    /// The underlying TCP SSL stream.
    beast::ssl_stream<beast::tcp_stream> stream_;

    char data_[1024];

    /// \brief CompletionToken for the asynchronous SSL handshake.
    void onHandshake(beast::error_code const& _error);

    /// \brief CompletionToken for the asynchronous SSL shutdown.
    void onShutdown(beast::error_code const& _error);

  public:
    /// \brief Create a secure TCP Session.
    /// \param _socket The raw TCP socket.
    /// \param _ctx A reference to the SSL context.
    /// \param _state The given SharedState.
    SSLTCPSession(asio::io_context& _ioc, tcp::socket&& _socket,
                  ssl::context& _ctx, std::shared_ptr<SharedState> _state);

    /// \brief Destructor.
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
