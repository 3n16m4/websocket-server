#ifndef WEBSOCKET_SERVER_PLAIN_TCP_SESSION_HH
#define WEBSOCKET_SERVER_PLAIN_TCP_SESSION_HH

#include "websocket_server/TCPSession.hh"

#include <boost/beast/core/tcp_stream.hpp>

namespace amadeus {
class PlainTCPSession
    : public TCPSession<PlainTCPSession>
    , public std::enable_shared_from_this<PlainTCPSession>
{
  private:
    /// The underlying TCP stream.
    beast::tcp_stream stream_;

  public:
    /// \brief Create a plain TCP Session.
    PlainTCPSession(beast::tcp_stream&& _stream, beast::flat_buffer&& _buffer,
                    std::shared_ptr<SharedState> const& _state);

    ~PlainTCPSession();

    /// \brief Return the underlying TCP stream.
    beast::tcp_stream& stream() noexcept;

    /// \brief Returns and moves ownership of the underlying TCP stream to the
    /// caller.
    beast::tcp_stream releaseStream() noexcept;

    /// \brief Starts the TCP Session.
    void run();

    /// \brief Ends the TCP Session.
    /// \note Called by the base class.
    void disconnect();
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_TCP_SESSION_HH