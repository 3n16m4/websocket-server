#include "websocket_server/SSLTCPSession.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/Logger.hh"

#include <string_view>

using namespace amadeus;

SSLTCPSession::SSLTCPSession(asio::io_context& _ioc, tcp::socket&& _socket,
                             ssl::context& _ctx,
                             std::shared_ptr<SharedState> _state)
    : TCPSession<SSLTCPSession>(_ioc, std::move(_state))
    , stream_(std::move(_socket), _ctx)
{
    LOG_DEBUG("SSLTCPSession::SSLTCPSession()\n");
}

SSLTCPSession::~SSLTCPSession()
{
    LOG_DEBUG("SSLTCPSession::~SSLTCPSession()\n");
}

beast::ssl_stream<beast::tcp_stream>& SSLTCPSession::stream() noexcept
{
    return stream_;
}

beast::ssl_stream<beast::tcp_stream> SSLTCPSession::releaseStream() noexcept
{
    return std::move(stream_);
}

void SSLTCPSession::run()
{
    beast::get_lowest_layer(stream_).expires_after(Timeout);

    // Perform the async SSL handshake.
    stream_.async_handshake(
        ssl::stream_base::server,
        [self = shared_from_this()](auto&& ec) { self->onHandshake(ec); });
}

void SSLTCPSession::disconnect()
{
    beast::get_lowest_layer(stream_).expires_after(Timeout);

    stream_.next_layer().cancel();

    // Perform the async SSL shutdown.
    stream_.async_shutdown(
        [self = shared_from_this()](auto&& ec) { self->onShutdown(ec); });
}

void SSLTCPSession::onHandshake(beast::error_code const& _error)
{
    if (_error) {
        LOG_ERROR("SSL Handshake error: {}\n", _error.message());
        return;
    }
    LOG_DEBUG("SSLTCPSession::onHandshake()\n");

    /*
    Beast’s detect_ssl is designed to work with beast’s http and websocket
    stream types. As you have seen, it is destructive with respect to the
    underlying tcp/ssl receive buffers.

    If we wanted ssl detection for a standard asio socket we have two options:

    1 write a version of detect_ssl that uses msg_peek when reading the
    underlying socket so that the data is not actually consumed from the socket
    when read.

    2 transfer the contents of the flat_buffer to an asio dynamic_buffer_v2 and
    use that buffer for subsequent asio operations on the stream.

    Beast and asio dynamic buffers are not currently compatible. I have a branch
    which is working towards unification but it’s not ready yet.
    */
    TCPSession::run();
}

void SSLTCPSession::onShutdown(beast::error_code const& _error)
{
    if (_error) {
        LOG_ERROR("SSL Shutdown error: {}\n", _error.message());
        return;
    }

    // Close the underlying TCP stream.
    beast::error_code ec;
    stream_.next_layer().socket().close(ec);

    if (ec) {
        LOG_ERROR("close error: {}\n", ec.message());
    }

    LOG_DEBUG("SSLTCPSession gracefully shut down.\n");
}
