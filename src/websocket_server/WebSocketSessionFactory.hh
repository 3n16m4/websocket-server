#ifndef WEBSOCKET_SERVER_WEBSOCKET_SESSION_FACTORY_HH
#define WEBSOCKET_SERVER_WEBSOCKET_SESSION_FACTORY_HH

#include "websocket_server/asiofwd.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

#include <memory>

namespace amadeus {
/// Forward declarations
class SharedState;
class PlainWebSocketSession;
class SSLWebSocketSession;

/// \brief Factory method for creating a PlainWebSocketSession by a given
/// tcp_stream and SharedState.
std::shared_ptr<PlainWebSocketSession>
make_websocket_session(beast::tcp_stream _stream,
                       std::shared_ptr<SharedState> _state);

/// \brief Factory method for creating an SSLWebSocketSession by a given
/// tcp_stream and SharedState.
std::shared_ptr<SSLWebSocketSession>
make_websocket_session(beast::ssl_stream<beast::tcp_stream> _stream,
                       std::shared_ptr<SharedState> _state);

} // namespace amadeus

#endif // !WEBSOCKET_SERVER_WEBSOCKET_SESSION_FACTORY_HH
