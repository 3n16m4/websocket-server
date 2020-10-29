#ifndef WEBSOCKET_SERVER_WEBSOCKET_SESSION_PLAIN_HH
#define WEBSOCKET_SERVER_WEBSOCKET_SESSION_PLAIN_HH

#include "websocket_server/WebsocketSession.hh"

namespace amadeus {
class WebsocketSessionPlain
    : public WebsocketSession<WebsocketSessionPlain>
    , public std::enable_shared_from_this<WebsocketSessionPlain>
{
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_WEBSOCKET_SESSION_PLAIN_HH