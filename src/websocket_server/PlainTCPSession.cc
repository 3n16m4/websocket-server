#include "websocket_server/PlainTCPSession.hh"
#include "websocket_server/Logger.hh"

using namespace amadeus;

PlainTCPSession::PlainTCPSession(beast::tcp_stream&& _stream,
                                 beast::flat_buffer&& _buffer,
                                 std::shared_ptr<SharedState> const& _state)
    : TCPSession<PlainTCPSession>(std::move(_buffer), _state)
    , stream_(std::move(_stream))
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
    this->doRead();
}

void PlainTCPSession::disconnect()
{
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

    if (ec) {
        LOG_ERROR("shutdown error: {}\n", ec.message());
    }
}