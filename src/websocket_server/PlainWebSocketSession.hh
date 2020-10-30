#ifndef WEBSOCKET_SERVER_PLAIN_WEBSOCKET_SESSION_HH
#define WEBSOCKET_SERVER_PLAIN_WEBSOCKET_SESSION_HH

#include "websocket_server/WebSocketSession.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>

namespace amadeus {
class PlainWebSocketSession
    : public WebSocketSession<PlainWebSocketSession>
    , public std::enable_shared_from_this<PlainWebSocketSession>
{
  private:
    websocket::stream<beast::tcp_stream> ws_;

  public:
    /// \brief Constructor. Creates the plain session.
    explicit PlainWebSocketSession(beast::tcp_stream&& _stream)
        : ws_(std::move(_stream))
    {
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_WEBSOCKET_SESSION_HH