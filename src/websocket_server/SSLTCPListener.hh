#ifndef WEBSOCKET_SERVER_SSL_TCP_LISTENER_HH
#define WEBSOCKET_SERVER_SSL_TCP_LISTENER_HH

#include "websocket_server/Listener.hh"

namespace amadeus {
using SSLTCPListener = Listener<SSLTCPSession>;
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_TCP_LISTENER_HH