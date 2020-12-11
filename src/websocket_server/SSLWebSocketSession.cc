#include "websocket_server/SSLWebSocketSession.hh"

using namespace amadeus;

SSLWebSocketSession::SSLWebSocketSession(
    beast::ssl_stream<beast::tcp_stream>&& _stream,
    std::shared_ptr<SharedState> _state)
    : WebSocketSession<SSLWebSocketSession>(std::move(_state))
    , ws_(std::move(_stream))
{
}

websocket::stream<beast::ssl_stream<beast::tcp_stream>>&
SSLWebSocketSession::stream() noexcept
{
    return ws_;
}
