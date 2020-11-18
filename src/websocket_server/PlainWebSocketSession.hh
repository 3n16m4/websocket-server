#ifndef WEBSOCKET_SERVER_PLAIN_WEBSOCKET_SESSION_HH
#define WEBSOCKET_SERVER_PLAIN_WEBSOCKET_SESSION_HH

#include "websocket_server/WebSocketSession.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>

namespace amadeus {
class SharedState;
class PlainWebSocketSession
    : public WebSocketSession<PlainWebSocketSession>
    , public std::enable_shared_from_this<PlainWebSocketSession>
{
  private:
    /// The underlying WebSocket TCP stream.
    websocket::stream<beast::tcp_stream> ws_;

  public:
    /// \brief Constructor. Creates the Plain WebSocketSession.
    explicit PlainWebSocketSession(beast::tcp_stream&& _stream,
                                   std::shared_ptr<SharedState> const& _state);

    /// \brief Returns the underlying WebSocket stream.
    /// \note Called by the base class.
    websocket::stream<beast::tcp_stream>& stream() noexcept;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_WEBSOCKET_SESSION_HH