#ifndef WEBSOCKET_SERVER_SSL_HTTP_SESSION_HH
#define WEBSOCKET_SERVER_SSL_HTTP_SESSION_HH

#include "websocket_server/HttpSession.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

namespace amadeus {
/// \brief Describes a simple SSLHttpSession which derives from the CRTP
/// HTTPSession class. Represents a single SSL Http Session on the network.
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
    /// \brief Create a Secure HTTP Session.
    /// \param _socket The raw TCP socket from which the Http session is created
    /// with.
    /// \param _ctx A reference to the SSL context.
    /// \param _state The given SharedState.
    SSLHttpSession(tcp::socket&& _socket, ssl::context& _ctx,
                   std::shared_ptr<SharedState> _state);

    /// \brief Destructor.
    ~SSLHttpSession();

    /// \brief Return the underlying TCP stream.
    beast::ssl_stream<beast::tcp_stream>& stream() noexcept;

    /// \brief Returns and moves ownership of the underlying TCP stream to the
    /// caller.
    beast::ssl_stream<beast::tcp_stream> releaseStream() noexcept;

    /// \brief Starts the SSL HTTP Session.
    void run();

    /// \brief Ends the SSL HTTP Session.
    /// \note Called by the base class.
    void disconnect();
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_HTTP_SESSION_HH
