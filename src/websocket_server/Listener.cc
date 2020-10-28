#include "websocket_server/Listener.hh"

#include <boost/asio/strand.hpp>

using namespace amadeus;

Listener::Listener(asio::io_context& _io, tcp::endpoint _endpoint)
    : io_(_io)
    , acceptor_(asio::make_strand(_io))
    , endpoint_(std::move(_endpoint))
{
}

void Listener::run()
{
    // Open the acceptor, reuse same address, bind server address and start
    // listening for new connections.
    acceptor_.open(endpoint_.protocol());
    acceptor_.set_option(asio::socket_base::reuse_address(true));
    acceptor_.bind(endpoint_);
    acceptor_.listen(asio::socket_base::max_listen_connections);

    // Every new connections gets its own strand to make sure that their
    // handlers are not executed concurrently.
    acceptor_.async_accept(
        asio::make_strand(io_),
        [this](auto&& ec, auto&& socket) { onAccept(ec, std::move(socket)); });
}

void Listener::onAccept(beast::error_code const& _error, tcp::socket&& _socket)
{
    if (_error) {
        /// TODO: Handle error properly.
    } else {
        /// TODO: Create a new session and give up ownership to the socket.

        // Every new connections gets its own strand to make sure that their
        // handlers are not executed concurrently.
        acceptor_.async_accept(asio::make_strand(io_),
                               [this](auto&& ec, auto&& socket) {
                                   onAccept(ec, std::move(socket));
                               });
    }
}