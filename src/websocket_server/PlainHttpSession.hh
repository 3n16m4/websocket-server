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
    PlainHttpSession(tcp::socket&& _socket,
                     std::shared_ptr<SharedState> _state);

    ~PlainHttpSession();

    /// \brief Return the underlying TCP stream.
    beast::tcp_stream& stream() noexcept;

    /// \brief Returns and moves ownership of the underlying TCP stream to the
    /// caller.
    beast::tcp_stream releaseStream() noexcept;

    /// \brief Starts the HTTP Session.
    void run();

    /// \brief Ends the HTTP Session.
    /// \note Called by the base class.
    void disconnect();
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_HTTP_SESSION_HH
