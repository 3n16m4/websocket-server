#include "websocket_server/DetectSession.hh"

#include <boost/asio/dispatch.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/detect_ssl.hpp>

using namespace amadeus;

DetectSession::DetectSession(tcp::socket&& _socket, ssl::context& _ctx,
                             std::shared_ptr<SharedState> const& _state)
    : stream_(std::move(_socket))
    , ctx_(_ctx)
    , state_(_state)
{
}

void DetectSession::run()
{
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    asio::dispatch(stream_.get_executor(),
                   [self = shared_from_this()]() { self->onRun(); });
}

void DetectSession::onRun()
{
    // Set a timeout.
    stream_.expires_after(std::chrono::seconds(10));

    beast::async_detect_ssl(stream_, buffer_,
                            [self = shared_from_this()](
                                beast::error_code const& error, bool result) {
                                self->onDetect(error, result);
                            });
}

void DetectSession::onDetect(beast::error_code const& _error, bool _result)
{
    if (_error) {
        /// TODO: Handle error correctly...
        return;
    }

    if (_result) {
        // An SSL connection was detected.
        /// TODO: Launch SSL session.
    } else {
        // A plain (non-secure) session was detected.
        /// TODO: Launch plain session.
    }
}