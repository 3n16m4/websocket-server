#ifndef WEBSOCKET_SERVER_SSL_WEBSOCKET_SESSION_HH
#define WEBSOCKET_SERVER_SSL_WEBSOCKET_SESSION_HH

#include "websocket_server/WebSocketSession.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

namespace amadeus {
/// Forward declaration.
class SharedState;
/// \brief Describes a simple SSLWebSocketSession which derives from the CRTP
/// WebSocketSession class. Represents a single Secure WebSocket Session on the
/// network which describes an open connection from the JavaScript frontend.
class SSLWebSocketSession
    : public WebSocketSession<SSLWebSocketSession>
    , public std::enable_shared_from_this<SSLWebSocketSession>
{
  private:
    /// The underlying SSL WebSocket TCP stream.
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;

  public:
    /// \brief Constructor. Creates the SSL WebSocketSession.
    /// \param _stream The raw secure tcp stream from which the WebSocket
    /// session is created from.
    /// \param _state The SharedState.
    SSLWebSocketSession(beast::ssl_stream<beast::tcp_stream>&& _stream,
                        std::shared_ptr<SharedState> _state);

    /// \brief Returns the underlying SSL WebSocket stream.
    /// \note Called by the base class.
    websocket::stream<beast::ssl_stream<beast::tcp_stream>>& stream() noexcept;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_WEBSOCKET_SESSION_HH
