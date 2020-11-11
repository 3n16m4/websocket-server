#ifndef WEBSOCKET_SERVER_WEBSOCKET_SESSION_HH
#define WEBSOCKET_SERVER_WEBSOCKET_SESSION_HH

#include "websocket_server/asiofwd.hh"

#include <boost/beast/http/message.hpp>

#include <memory>
#include <string>

namespace amadeus {
/// CRTP is used here to avoid code duplication and virtual function calls.
/// This is not only beneficial for performance but also to allow SSL websocket
/// sessions and regular websocket sessions to work with the same code.
/// \brief Provides a simple default WebSocketSession implementation.
template <class Derived>
class WebSocketSession
{
  private:
    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

  public:
    /// \brief Start the asynchronous operation.
    template <class Body, class Allocator>
    void run(http::request<Body, http::basic_fields<Allocator>> _req)
    {
        // Accept the WebSocket upgrade request
        // doAccept(std::move(_req));
    }

    void send(std::shared_ptr<std::string const> const& _message)
    {
        // ... some queue work
        // send the the message response.
    }

  protected:
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_WEBSOCKET_SESSION_HH