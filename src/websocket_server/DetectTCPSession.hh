#ifndef WEBSOCKET_SERVER_DETECT_TCP_SESSION_HH
#define WEBSOCKET_SERVER_DETECT_TCP_SESSION_HH

#include "websocket_server/DetectSession.hh"
#include "websocket_server/PlainTCPSession.hh"
#include "websocket_server/SSLTCPSession.hh"

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
                     std::shared_ptr<SharedState> const& _state)
        : DetectSession<DetectTCPSession>(std::move(_socket), _ctx, _state)
    {
    }

    /// \brief The CompletionToken for the SSL detection process.
    /// Will either launch a PlainTCPSession or SSLTCPSession.
    /// \note Called from the base class.
    /// \param _result true if SSL detected, otherwise false.
    void onDetect(bool _result)
    {
        if (_result) {
            // An SSL connection was detected.
            std::make_shared<SSLTCPSession>(
                std::move(this->stream_), this->ctx_, std::move(this->buffer_),
                this->state_)
                ->run();
        } else {
            // A plain (non-secure) session was detected.
            std::make_shared<PlainTCPSession>(std::move(this->stream_),
                                              std::move(this->buffer_),
                                              this->state_)
                ->run();
        }
    }

    /// \brief Starts the SSL detector for the TCP Sessions.
    void run()
    {
        this->doRead();
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_DETECT_TCP_SESSION_HH