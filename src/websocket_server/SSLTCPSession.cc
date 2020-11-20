#include "websocket_server/SSLTCPSession.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/Logger.hh"

#include <string_view>

using namespace amadeus;

SSLTCPSession::SSLTCPSession(beast::tcp_stream&& _stream, ssl::context& _ctx,
                             beast::flat_buffer&& _buffer,
                             std::shared_ptr<SharedState> const& _state)
    : TCPSession<SSLTCPSession>(std::move(_buffer), _state)
    , stream_(std::move(_stream), _ctx)
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
    if (_error) {
        LOG_ERROR("SSL Handshake error: {}\n", _error.message());
        return;
    }
    LOG_DEBUG("SSLTCPSession::onHandshake()\n");

    // Consume the portion of the buffer used by the handshake
    buffer_.consume(_bytesTransferred);

    /*
    Beast’s detect_ssl is designed to work with beast’s http and websocket
    stream types. As you have seen, it is destructive with respect to the underlying
    tcp/ssl receive buffers.

    If we wanted ssl detection for a standard asio socket we have two options:

    1 write a version of detect_ssl that uses msg_peek when reading the underlying
    socket so that the data is not actually consumed from the socket when read.

    2 transfer the contents of the flat_buffer to an asio dynamic_buffer_v2 and use
    that buffer for subsequent asio operations on the stream.

    Beast and asio dynamic buffers are not currently compatible. I have a branch
    which is working towards unification but it’s not ready yet.
    */
    asio::async_read(stream_, asio::buffer(buffer_.data(), buffer_.size()),
                     [this, self = shared_from_this()](auto&& e, auto&& l) {
                         LOG_DEBUG("Received: {}\n", e.message());
                     });
    /*stream_.async_read_some(
        boost::asio::buffer(data_),
        [this, self](const boost::system::error_code& ec, std::size_t length) {
            if (!ec) {
                LOG_DEBUG("Received: {}\n", data_);
                this->doRead();
            }
        });*/

    /*asio::async_read(
        stream_, buffer_,
        [this, self = shared_from_this()](auto&& ec, auto&& bytes_transferred) {
            this->doRead();
        });*/
}

void SSLTCPSession::onShutdown(beast::error_code const& _error)
{
    if (_error) {
        LOG_ERROR("SSL Shutdown error: {}\n", _error.message());
        return;
    }

    // Close the underlying TCP stream.
    stream_.next_layer().close();

    LOG_DEBUG("SSLTCPSession gracefully shut down.\n");
}