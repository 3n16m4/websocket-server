#ifndef WEBSOCKET_SERVER_LISTENER_HH
#define WEBSOCKET_SERVER_LISTENER_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/SharedState.hh"
#include "websocket_server/DetectHttpSession.hh"

#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>

#include <type_traits>

namespace amadeus {
/// \brief This class is responsible for accepting incoming connections. It is
/// important to note, that this class does not 'manage' any newly created
/// connection by any means. In fact, the connection itself is responsible for
/// its own lifetime management. This is done by creating shared pointers for
/// each connection and making the connection itself inherit from
/// enable_shared_from_this.
template <typename ProtocolDetector>
class Listener
{
  private:
    /// A reference to the main IO-Context.
    asio::io_context& io_;
    /// A reference to the main SSL-Context.
    ssl::context& ctx_;
    /// The TCP acceptor.
    tcp::acceptor acceptor_;
    /// The endpoint associated to the TCP acceptor.
    tcp::endpoint endpoint_;
    /// The shared state for each session.
    std::shared_ptr<SharedState> state_;

    /// \brief Accepts new incoming connections.
    void doAccept()
    {
        // Every new connections gets its own strand to make sure that their
        // handlers are not executed concurrently.
        acceptor_.async_accept(asio::make_strand(io_),
                               [this](auto&& ec, auto&& socket) {
                                   onAccept(ec, std::move(socket));
                               });
    }

    /// \brief Called each time a new connection was accepted.
    void onAccept(beast::error_code const& _error, tcp::socket&& _socket)
    {
        if (_error) {
            /// TODO: Handle error properly.
        } else {
            // Create an HTTP / TCP session detector, give up ownership to the
            // socket and run it.
            std::make_shared<ProtocolDetector>(std::move(_socket), ctx_, state_)
                ->run();

            // Accept another connection.
            doAccept();
        }
    }

  public:
    /// \brief Constructor.
    /// \param _io A reference to the main IO-Context.
    /// \param _ctx A reference to the main SSL-Context.
    /// \param _endpoint The endpoint to which the tcp acceptor will listen to.
    Listener(asio::io_context& _io, ssl::context& _ctx, tcp::endpoint _endpoint,
             std::shared_ptr<SharedState> const& _state)
        : io_(_io)
        , ctx_(_ctx)
        , acceptor_(asio::make_strand(_io))
        , endpoint_(std::move(_endpoint))
        , state_(_state)
    {
    }

    /// \brief Starts the listener and accepting incoming connections.
    /// \throw Any exception thrown by boost.
    void run()
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
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_LISTENER_HH