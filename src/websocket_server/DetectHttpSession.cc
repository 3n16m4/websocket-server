#include "websocket_server/DetectHttpSession.hh"
#include "websocket_server/PlainHttpSession.hh"
#include "websocket_server/SSLHttpSession.hh"

using namespace amadeus;

DetectHttpSession::DetectHttpSession(tcp::socket&& _socket, ssl::context& _ctx,
                                     std::shared_ptr<SharedState> const& _state)
    : DetectSession<DetectHttpSession>(std::move(_socket), _ctx, _state)
{
}

void DetectHttpSession::onDetectSSL(bool _result)
{
    if (_result) {
        // An SSL connection was detected.
        std::make_shared<SSLHttpSession>(std::move(this->stream_), this->ctx_,
                                         std::move(this->buffer_), this->state_)
            ->run();
    } else {
        // A plain (non-secure) session was detected.
        std::make_shared<PlainHttpSession>(
            std::move(this->stream_), std::move(this->buffer_), this->state_)
            ->run();
    }
}

void DetectHttpSession::run()
{
    this->doRead();
}