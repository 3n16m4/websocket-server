#include "websocket_server/SSLHttpSession.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/Logger.hh"

using namespace amadeus;

SSLHttpSession::SSLHttpSession(tcp::socket&& _socket, ssl::context& _ctx,
                               std::shared_ptr<SharedState> _state)
    : HttpSession<SSLHttpSession>(std::move(_state))
    , stream_(std::move(_socket), _ctx)
{
    LOG_DEBUG("SSLHttpSession::SSLHttpSession()\n");
}

SSLHttpSession::~SSLHttpSession()
{
    LOG_DEBUG("SSLHttpSession::~SSLHttpSession()\n");
}

beast::ssl_stream<beast::tcp_stream>& SSLHttpSession::stream() noexcept
{
    return stream_;
}

beast::ssl_stream<beast::tcp_stream> SSLHttpSession::releaseStream() noexcept
{
    return std::move(stream_);
}

void SSLHttpSession::run()
{
    beast::get_lowest_layer(stream_).expires_after(Timeout);

    // Perform the async SSL handshake.
    stream_.async_handshake(
        ssl::stream_base::server, buffer_.data(),
        [self = shared_from_this()](auto&& ec, auto&& bytes_transferred) {
            self->onHandshake(ec, bytes_transferred);
        });
}

void SSLHttpSession::disconnect()
{
    beast::get_lowest_layer(stream_).expires_after(Timeout);

    // Perform the async SSL shutdown.
    stream_.async_shutdown(
        [self = shared_from_this()](auto&& ec) { self->onShutdown(ec); });
}

void SSLHttpSession::onHandshake(beast::error_code const& _error,
                                 std::size_t _bytesTransferred)
{
    if (_error) {
        LOG_ERROR("SSL Handshake error: {}\n", _error.message());
        return;
    }

    LOG_DEBUG("SSLHttpSession::onHandshake()\n");

    // Consume the portion of the buffer used by the handshake
    buffer_.consume(_bytesTransferred);

    this->doRead();
}

void SSLHttpSession::onShutdown(beast::error_code const& _error)
{
    if (_error) {
        LOG_ERROR("SSL Shutdown error: {}\n", _error.message());
        return;
    }

    // Close the underlying TCP stream.
    stream_.next_layer().close();

    LOG_DEBUG("SSLHttpSession gracefully shut down.\n");
}
