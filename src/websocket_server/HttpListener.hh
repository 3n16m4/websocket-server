#ifndef WEBSOCKET_SERVER_HTTP_LISTENER_HH
#define WEBSOCKET_SERVER_HTTP_LISTENER_HH

#include "websocket_server/Listener.hh"
#include "websocket_server/DetectHttpSession.hh"

namespace amadeus {
using HttpListener = Listener<DetectHttpSession>;
}

#endif // !WEBSOCKET_SERVER_HTTP_LISTENER_HH