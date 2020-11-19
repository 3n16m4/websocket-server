#include "websocket_server/PlainWebSocketSession.hh"

using namespace amadeus;

PlainWebSocketSession::PlainWebSocketSession(
    beast::tcp_stream&& _stream, std::shared_ptr<SharedState> const& _state)
    : WebSocketSession<PlainWebSocketSession>(_state)
    , ws_(std::move(_stream))
{
}

websocket::stream<beast::tcp_stream>& PlainWebSocketSession::stream() noexcept
{
    return ws_;
}