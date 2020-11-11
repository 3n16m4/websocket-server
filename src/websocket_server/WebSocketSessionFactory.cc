#include "websocket_server/WebSocketSessionFactory.hh"
#include "websocket_server/PlainWebSocketSession.hh"
#include "websocket_server/SSLWebSocketSession.hh"

namespace amadeus {
std::shared_ptr<PlainWebSocketSession>
make_websocket_session(beast::tcp_stream _stream)
{
    return std::make_shared<PlainWebSocketSession>(std::move(_stream));
}

std::shared_ptr<SSLWebSocketSession>
make_websocket_session(beast::ssl_stream<beast::tcp_stream> _stream)
{
    return std::make_shared<SSLWebSocketSession>(std::move(_stream));
}
}