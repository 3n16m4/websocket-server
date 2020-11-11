#include "websocket_server/SSLHttpSession.hh"
#include "websocket_server/Common.hh"

using namespace amadeus;

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
        /// TODO: error handling...
        return;
    }

    std::cout << "SSLHttpSession::onHandshake()\n";

    // Consume the portion of the buffer used by the handshake
    buffer_.consume(_bytesTransferred);

    this->doRead();
}

void SSLHttpSession::onShutdown(beast::error_code const& _error)
{
    throw std::runtime_error("NotImplemented");
}