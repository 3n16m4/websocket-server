#ifndef WEBSOCKET_SERVER_WEBSOCKET_SESSION_SSL_HH
#define WEBSOCKET_SERVER_WEBSOCKET_SESSION_SSL_HH

#include "websocket_server/WebsocketSession.hh"

namespace amadeus {
class WebsocketSessionSSL
    : public WebsocketSession<WebsocketSessionSSL>
    , std::enable_shared_from_this<WebsocketSessionSSL>
{
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_WEBSOCKET_SESSION_SSL_HH