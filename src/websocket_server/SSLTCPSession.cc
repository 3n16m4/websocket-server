#include "websocket_server/SSLTCPSession.hh"
#include "websocket_server/Common.hh"

#include <string_view>

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
    if (_error) {
        std::cerr << "Handshake error: " << _error.message() << '\n';
        return;
    }
    std::cout << "SSLTCPSession::onHandshake()\n";

    // Consume the portion of the buffer used by the handshake
    buffer_.consume(_bytesTransferred);

    // this->doRead();

    auto self(shared_from_this());
    stream_.async_read_some(
        boost::asio::buffer(data_),
        [this, self](const boost::system::error_code& ec, std::size_t length) {
            if (!ec) {
                std::cout << ec.message() << " " << length << '\n';
                std::string_view const message{data_, length};
                std::cout << "Message: " << message << '\n';
                doWrite();
            }
        });
}

void SSLTCPSession::onShutdown(beast::error_code const& _error)
{
    throw std::runtime_error("NotImplemented");
}

void SSLTCPSession::onWrite(beast::error_code const& _error,
                            std::size_t _bytesTransferred)
{
}

void SSLTCPSession::doWrite()
{
    auto self(shared_from_this());
    asio::async_write(
        stream_, boost::asio::buffer(data_),
        [this, self](const boost::system::error_code& ec, std::size_t length) {
            if (!ec) {
                std::cout << ec.message() << " " << length << '\n';
                std::string_view const message{data_, length};
                std::cout << "Wrote: " << message << '\n';
            }
        });
}