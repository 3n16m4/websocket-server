#include "websocket_server/TCPListener.hh"
#include "websocket_server/DetectSession.hh"

#include <boost/asio/strand.hpp>

using namespace amadeus;

TCPListener::TCPListener(asio::io_context& _io, ssl::context& _ctx,
                         tcp::endpoint _endpoint)
    : io_(_io)
    , ctx_(_ctx)
    , acceptor_(asio::make_strand(_io))
    , endpoint_(std::move(_endpoint))
{
}

void TCPListener::doAccept()
{
    // Every new connections gets its own strand to make sure that their
    // handlers are not executed concurrently.
    acceptor_.async_accept(
        asio::make_strand(io_),
        [this](auto&& ec, auto&& socket) { onAccept(ec, std::move(socket)); });
}

void TCPListener::onAccept(beast::error_code const& _error,
                           tcp::socket&& _socket)
{
    if (_error) {
        /// TODO: Handle error properly.
    } else {
        // Create a session detector, give up ownership to the socket and run
        // it.
        std::make_shared<DetectSession>(std::move(_socket), ctx_, state_)
            ->run();

        // Accept another connection.
        doAccept();
    }
}

void TCPListener::run()
{
    // Open the acceptor, reuse same address, bind server address and start
    // listening for new connections.
    acceptor_.open(endpoint_.protocol());
    acceptor_.set_option(asio::socket_base::reuse_address(true));
    acceptor_.bind(endpoint_);
    acceptor_.listen(asio::socket_base::max_listen_connections);

    // Accept new connections.
    doAccept();
}
