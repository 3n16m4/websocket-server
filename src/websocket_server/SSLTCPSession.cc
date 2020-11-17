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

    // this->doRead();

    auto self(shared_from_this());
    stream_.async_read_some(
        boost::asio::buffer(data_),
        [this, self](const boost::system::error_code& ec, std::size_t length) {
            if (!ec) {
                doWrite(length);
            }
        });
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

void SSLTCPSession::onWrite(beast::error_code const& _error,
                            std::size_t _bytesTransferred)
{
}

void SSLTCPSession::doWrite(std::size_t _bytes)
{
    auto self(shared_from_this());
    asio::async_write(stream_, boost::asio::buffer(data_, _bytes),
                      [this, self, _bytes](const boost::system::error_code& ec,
                                           std::size_t length) {
                          if (!ec) {
                              LOG_DEBUG("{} {}\n", ec.message(), _bytes);
                              std::string_view const message{data_, _bytes};
                              LOG_DEBUG("Wrote: {}\n", message);
                          }
                      });
}