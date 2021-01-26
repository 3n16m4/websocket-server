#ifndef WEBSOCKET_SERVER_SSL_HTTP_LISTENER_HH
#define WEBSOCKET_SERVER_SSL_HTTP_LISTENER_HH

#include "websocket_server/Listener.hh"

namespace amadeus {
/// Alias for a Secure HTTP Listener.
using SSLHttpListener = Listener<SSLHttpSession>;
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_HTTP_LISTENER_HH