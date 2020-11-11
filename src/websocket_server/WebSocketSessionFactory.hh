#ifndef WEBSOCKET_SERVER_WEBSOCKET_SESSION_FACTORY_HH
#define WEBSOCKET_SERVER_WEBSOCKET_SESSION_FACTORY_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/PlainWebSocketSession.hh"
#include "websocket_server/SSLWebSocketSession.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

#include <memory>

namespace amadeus {
/// Forward declarations
class PlainWebSocketSession;
class SSLWebSocketSession;

std::shared_ptr<PlainWebSocketSession>
make_websocket_session(beast::tcp_stream _stream);

std::shared_ptr<SSLWebSocketSession>
make_websocket_session(beast::ssl_stream<beast::tcp_stream> _stream);

} // namespace amadeus

#endif // !WEBSOCKET_SERVER_WEBSOCKET_SESSION_FACTORY_HH