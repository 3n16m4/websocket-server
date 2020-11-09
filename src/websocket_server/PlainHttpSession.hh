#ifndef WEBSOCKET_SERVER_PLAIN_HTTP_SESSION_HH
#define WEBSOCKET_SERVER_PLAIN_HTTP_SESSION_HH

#include "websocket_server/HttpSession.hh"

#include <boost/beast/core/tcp_stream.hpp>

namespace amadeus {
class PlainHttpSession
    : public HttpSession<PlainHttpSession>
    , public std::enable_shared_from_this<PlainHttpSession>
{
  private:
    /// The underlying TCP stream.
    beast::tcp_stream stream_;

  public:
    /// \brief Create a plain HTTP Session.
    PlainHttpSession(beast::tcp_stream&& _stream, beast::flat_buffer&& _buffer,
                     std::shared_ptr<SharedState> const& _state)
        : HttpSession<PlainHttpSession>(std::move(_buffer), _state)
        , stream_(std::move(_stream))
    {
    }

    /// \brief Starts the HTTP Session.
    void run()
    {
        doRead();
    }

    /// \brief Ends the HTTP Session.
    /// \note Called by the base class.
    void disconnect();
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_HTTP_SESSION_HH