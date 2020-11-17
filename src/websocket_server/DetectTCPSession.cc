#include "websocket_server/DetectTCPSession.hh"
#include "websocket_server/PlainTCPSession.hh"
#include "websocket_server/SSLTCPSession.hh"

using namespace amadeus;

DetectTCPSession::DetectTCPSession(tcp::socket&& _socket, ssl::context& _ctx,
                                   std::shared_ptr<SharedState> const& _state)
    : DetectSession<DetectTCPSession>(std::move(_socket), _ctx, _state)
{
}

void DetectTCPSession::onDetectSSL(bool _result)
{
    if (_result) {
        // An SSL connection was detected.
        std::make_shared<SSLTCPSession>(std::move(this->stream_), this->ctx_,
                                        std::move(this->buffer_), this->state_)
            ->run();
    } else {
        // A plain (non-secure) session was detected.
        std::make_shared<PlainTCPSession>(
            std::move(this->stream_), std::move(this->buffer_), this->state_)
            ->run();
    }
}

void DetectTCPSession::run()
{
    this->doRead();
}