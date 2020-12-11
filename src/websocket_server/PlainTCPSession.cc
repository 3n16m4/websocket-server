#include "websocket_server/PlainTCPSession.hh"
#include "websocket_server/Logger.hh"

using namespace amadeus;

PlainTCPSession::PlainTCPSession(asio::io_context& _ioc, tcp::socket&& _socket,
                                 std::shared_ptr<SharedState> const& _state)
    : TCPSession<PlainTCPSession>(_ioc, _state)
    , stream_(std::move(_socket))
{
    LOG_DEBUG("PlainTCPSession::PlainTCPSession()\n");
}

PlainTCPSession::~PlainTCPSession()
{
    LOG_DEBUG("PlainTCPSession::~PlainTCPSession()\n");
}

beast::tcp_stream& PlainTCPSession::stream() noexcept
{
    return stream_;
}

beast::tcp_stream PlainTCPSession::releaseStream() noexcept
{
    return std::move(stream_);
}

void PlainTCPSession::run()
{
    TCPSession::run();
}

void PlainTCPSession::disconnect()
{
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

    if (ec) {
        LOG_ERROR("shutdown error: {}\n", ec.message());
    }

    stream_.socket().close(ec);

    if (ec) {
        LOG_ERROR("close error: {}\n", ec.message());
    }
}
