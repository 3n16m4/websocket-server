#ifndef WEBSOCKET_SERVER_DETECT_HTTP_SESSION_HH
#define WEBSOCKET_SERVER_DETECT_HTTP_SESSION_HH

#include "websocket_server/DetectSession.hh"
#include "websocket_server/PlainHttpSession.hh"
#include "websocket_server/SSLHttpSession.hh"

#include <memory>

namespace amadeus {
class DetectHttpSession
    : public DetectSession<DetectHttpSession>
    , public std::enable_shared_from_this<DetectHttpSession>
{
  private:
  public:
    /// \brief Constructor.
    DetectHttpSession(tcp::socket&& _socket, ssl::context& _ctx,
                      std::shared_ptr<SharedState> const& _state)
        : DetectSession<DetectHttpSession>(std::move(_socket), _ctx, _state)
    {
    }

    /// \brief The CompletionToken for the SSL detection process.
    /// Will either launch a PlainHttpSession or SSLHttpSession.
    /// \note Called from the base class.
    /// \param _result true if SSL detected, otherwise false.
    void onDetect(bool _result)
    {
        if (_result) {
            // An SSL connection was detected.
            std::make_shared<SSLHttpSession>(
                std::move(this->stream_), this->ctx_, std::move(this->buffer_),
                this->state_)
                ->run();
        } else {
            // A plain (non-secure) session was detected.
            std::make_shared<PlainHttpSession>(std::move(this->stream_),
                                               std::move(this->buffer_),
                                               this->state_)
                ->run();
        }
    }

    /// \brief Starts the SSL detector for the HTTP Sessions.
    void run()
    {
        this->doRead();
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_DETECT_HTTP_SESSION_HH