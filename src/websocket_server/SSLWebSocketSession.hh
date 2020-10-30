#ifndef WEBSOCKET_SERVER_SSL_WEBSOCKET_SESSION_HH
#define WEBSOCKET_SERVER_SSL_WEBSOCKET_SESSION_HH

#include "websocket_server/WebsocketSession.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

namespace amadeus {
class SSLWebSocketSession
    : public WebSocketSession<SSLWebSocketSession>
    , public std::enable_shared_from_this<SSLWebSocketSession>
{
  private:
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;

  public:
    explicit SSLWebSocketSession(beast::ssl_stream<beast::tcp_stream>&& _stream)
        : ws_(std::move(_stream))
    {
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_WEBSOCKET_SESSION_HH