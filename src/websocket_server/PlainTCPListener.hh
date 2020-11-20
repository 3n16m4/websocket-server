#ifndef WEBSOCKET_SERVER_PLAIN_TCP_LISTENER_HH
#define WEBSOCKET_SERVER_PLAIN_TCP_LISTENER_HH

#include "websocket_server/Listener.hh"

namespace amadeus {
using PlainTCPListener = Listener<PlainTCPSession>;
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_TCP_LISTENER_HH