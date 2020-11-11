#include "websocket_server/SSLTCPSession.hh"
#include "websocket_server/Common.hh"

using namespace amadeus;

void SSLTCPSession::run()
{
    beast::get_lowest_layer(stream_).expires_after(Timeout);

    // Perform the async SSL handshake.
    stream_.async_handshake(
        ssl::stream_base::server, buffer_.data(),
        [self = shared_from_this()](auto&& ec, auto&& bytes_transferred) {
            self->onHandshake(ec, bytes_transferred);
        });
}

void SSLTCPSession::disconnect()
{
    beast::get_lowest_layer(stream_).expires_after(Timeout);

    // Perform the async SSL shutdown.
    stream_.async_shutdown(
        [self = shared_from_this()](auto&& ec) { self->onShutdown(ec); });
}

void SSLTCPSession::onHandshake(beast::error_code const& _error,
                                std::size_t _bytesTransferred)
{
    std::cout << "SSLTCPSession::onHandshake()\n";
}

void SSLTCPSession::onShutdown(beast::error_code const& _error)
{
    throw std::runtime_error("NotImplemented");
}