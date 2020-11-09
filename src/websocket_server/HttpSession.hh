#ifndef WEBSOCKET_SERVER_HTTP_SESSION_HH
#define WEBSOCKET_SERVER_HTTP_SESSION_HH

#include "websocket_server/asiofwd.hh"

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/string_body.hpp>

#include <optional>
#include <memory>
#include <string>

namespace amadeus {
/// CRTP is used here to avoid code duplication and virtual function calls.
/// This is not only beneficial for performance but also to allow SSL
/// HttpSessions and regular HttpSessions to work with the same code.
/// \brief Provides a simple default WebSocketSession implementation.
class SharedState;
template <class Derived>
class HttpSession
{
  private:
    /// The shared state.
    std::shared_ptr<SharedState> state_;
    /// The HTTP Request parser.
    std::optional<http::request_parser<http::string_body>> parser_;

    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief CompletionToken for the asynchronous read operation.
    void onRead(beast::error_code const& _ec, std::size_t _bytesTransferred);

    /// \brief CompletionToken for the asynchronous write operation.
    void onWrite(beast::error_code const& _ec, std::size_t _bytesTransferred);

  public:
    /// \brief Constructor.
    HttpSession(beast::flat_buffer&& _buffer,
                std::shared_ptr<SharedState> const& _state)
        : buffer_(std::move(_buffer))
        , state_(_state)
    {
    }

    /// \brief Starts the asynchronous read operation.
    void doRead();

    /// \brief Starts the asynchronous write operation.
    void doWrite();

  protected:
    /// The underlying buffer for incoming HTTP Requests.
    beast::flat_buffer buffer_;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_HTTP_SESSION_HH