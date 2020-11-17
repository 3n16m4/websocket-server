#ifndef WEBSOCKET_SERVER_DETECT_TCP_SESSION_HH
#define WEBSOCKET_SERVER_DETECT_TCP_SESSION_HH

#include "websocket_server/DetectSession.hh"

#include <memory>

namespace amadeus {
class DetectTCPSession
    : public DetectSession<DetectTCPSession>
    , public std::enable_shared_from_this<DetectTCPSession>
{
  private:
  public:
    /// \brief Constructor.
    DetectTCPSession(tcp::socket&& _socket, ssl::context& _ctx,
                     std::shared_ptr<SharedState> const& _state);

    /// \brief The CompletionToken for the SSL detection process.
    /// Will either launch a PlainTCPSession or SSLTCPSession.
    /// \note Called from the base class.
    /// \param _result true if SSL detected, otherwise false.
    void onDetectSSL(bool _result);

    /// \brief Starts the SSL detector for the TCP Sessions.
    void run();
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_DETECT_TCP_SESSION_HH