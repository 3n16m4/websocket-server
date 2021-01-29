#ifndef WEBSOCKET_SERVER_PLAIN_WEBSOCKET_SESSION_HH
#define WEBSOCKET_SERVER_PLAIN_WEBSOCKET_SESSION_HH

#include "websocket_server/WebSocketSession.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>

namespace amadeus {
/// Forward declaration.
class SharedState;
/// \brief Describes a simple PlainWebSocketSession which derives from the CRTP
/// WebSocketSession class. Represents a single Plain WebSocket Session on the
/// network which describes an open connection from the JavaScript frontend.
class PlainWebSocketSession
    : public WebSocketSession<PlainWebSocketSession>
    , public std::enable_shared_from_this<PlainWebSocketSession>
{
  private:
    /// The underlying WebSocket TCP stream.
    websocket::stream<beast::tcp_stream> ws_;

  public:
    /// \brief Constructor. Creates the Plain WebSocketSession.
    /// \param _stream The raw tcp stream from which the WebSocket session is
    /// created from.
    /// \param _state The SharedState.
    PlainWebSocketSession(beast::tcp_stream&& _stream,
                          std::shared_ptr<SharedState> _state);

    /// \brief Destructor.
    ~PlainWebSocketSession();

    /// \brief Returns the underlying WebSocket stream.
    /// \note Called by the base class.
    websocket::stream<beast::tcp_stream>& stream() noexcept;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_WEBSOCKET_SESSION_HH
