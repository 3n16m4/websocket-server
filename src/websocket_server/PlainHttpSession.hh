#ifndef WEBSOCKET_SERVER_PLAIN_HTTP_SESSION_HH
#define WEBSOCKET_SERVER_PLAIN_HTTP_SESSION_HH

#include "websocket_server/HttpSession.hh"

#include <boost/beast/core/tcp_stream.hpp>

namespace amadeus {
/// \brief Describes a simple PlainHttpSession which derives from the CRTP
/// HTTPSession class. Represents a single Plain Http Session on the network.
class PlainHttpSession
    : public HttpSession<PlainHttpSession>
    , public std::enable_shared_from_this<PlainHttpSession>
{
  private:
    /// The underlying TCP stream.
    beast::tcp_stream stream_;

  public:
    /// \brief Create a Plain HTTP Session.
    /// \param _socket The raw TCP socket from which the Http session is created
    /// with.
    /// \param _state The given SharedState.
    PlainHttpSession(tcp::socket&& _socket,
                     std::shared_ptr<SharedState> _state);

    /// \brief Destructor.
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
