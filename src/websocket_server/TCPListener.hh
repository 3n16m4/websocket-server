#ifndef WEBSOCKET_SERVER_TCP_LISTENER_HH
#define WEBSOCKET_SERVER_TCP_LISTENER_HH

#include "websocket_server/Listener.hh"
#include "websocket_server/DetectTCPSession.hh"

namespace amadeus {
using TCPListener = Listener<DetectTCPSession>;
}

#endif // !WEBSOCKET_SERVER_TCP_LISTENER_HH