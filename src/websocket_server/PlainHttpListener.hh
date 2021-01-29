#ifndef WEBSOCKET_SERVER_PLAIN_HTTP_LISTENER_HH
#define WEBSOCKET_SERVER_PLAIN_HTTP_LISTENER_HH

#include "websocket_server/Listener.hh"

namespace amadeus {
/// Alias for a Plain Http Listener.
using PlainHttpListener = Listener<PlainHttpSession>;
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_HTTP_LISTENER_HH