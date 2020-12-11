#ifndef WEBSOCKET_SERVER_SSL_WEBSOCKET_SESSION_HH
#define WEBSOCKET_SERVER_SSL_WEBSOCKET_SESSION_HH

#include "websocket_server/WebSocketSession.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

namespace amadeus {
class SharedState;
class SSLWebSocketSession
    : public WebSocketSession<SSLWebSocketSession>
    , public std::enable_shared_from_this<SSLWebSocketSession>
{
  private:
    /// The underlying SSL WebSocket TCP stream.
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;

  public:
    /// \brief Constructor. Creates the SSL WebSocketSession.
    SSLWebSocketSession(beast::ssl_stream<beast::tcp_stream>&& _stream,
                        std::shared_ptr<SharedState> _state);

    /// \brief Returns the underlying SSL WebSocket stream.
    /// \note Called by the base class.
    websocket::stream<beast::ssl_stream<beast::tcp_stream>>& stream() noexcept;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_WEBSOCKET_SESSION_HH
