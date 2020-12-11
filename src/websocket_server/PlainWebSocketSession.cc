#include "websocket_server/PlainWebSocketSession.hh"
#include "websocket_server/Logger.hh"

using namespace amadeus;

PlainWebSocketSession::PlainWebSocketSession(
    beast::tcp_stream&& _stream, std::shared_ptr<SharedState> _state)
    : WebSocketSession<PlainWebSocketSession>(std::move(_state))
    , ws_(std::move(_stream))
{
    LOG_DEBUG("PlainWebSocketSession::PlainWebSocketSession()\n");
}

PlainWebSocketSession::~PlainWebSocketSession()
{
    LOG_DEBUG("PlainWebSocketSession::~PlainWebSocketSession()\n");
}

websocket::stream<beast::tcp_stream>& PlainWebSocketSession::stream() noexcept
{
    return ws_;
}
