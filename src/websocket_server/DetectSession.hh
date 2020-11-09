#ifndef WEBSOCKET_SERVER_DETECT_SESSION_HH
#define WEBSOCKET_SERVER_DETECT_SESSION_HH

#include "websocket_server/asiofwd.hh"

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/flat_buffer.hpp>

#include <memory>

namespace amadeus {
/// Forward declarations
class SharedState;
/// \brief Helper class to detect SSL handshakes. If the session is secured, it
/// will create a new SSLTCPSession, otherwise it will spawn a new TCPSession.
class DetectSession : public std::enable_shared_from_this<DetectSession>
{
  private:
    /// A reference to the main SSL-Context.
    ssl::context& ctx_;
    /// The shared state.
    std::shared_ptr<SharedState> state_;
    /// The main TCP Stream.
    beast::tcp_stream stream_;
    /// The underlying buffer for determing an SSL request.
    beast::flat_buffer buffer_;

    /// \brief Detect a TLS/SSL handshake asynchronously on a stream.
    void onRun();

    /// \brief The CompletionToken for the SSL detection process.
    void onDetect(beast::error_code const& _error, bool _result);

  public:
    /// \brief Constructor.
    DetectSession(tcp::socket&& _socket, ssl::context& _ctx,
                  std::shared_ptr<SharedState> const& _state);

    /// \brief Starts the SSL detector.
    void run();
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_DETECT_SESSION_HH