#ifndef WEBSOCKET_SERVER_LISTENER_HH
#define WEBSOCKET_SERVER_LISTENER_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/SharedState.hh"
#include "websocket_server/PlainHttpSession.hh"
#include "websocket_server/PlainTCPSession.hh"
#include "websocket_server/SSLHttpSession.hh"
#include "websocket_server/SSLTCPSession.hh"

#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>

#include <type_traits>
#include <optional>

namespace amadeus {
/// \brief This class is responsible for accepting incoming connections. It is
/// important to note, that this class does not 'manage' any newly created
/// connection by any means. In fact, the connection itself is responsible for
/// its own lifetime management. This is done by creating shared pointers for
/// each connection and making the connection itself inherit from
/// enable_shared_from_this.
template <typename Protocol>
class Listener
{
  protected:
    /// A reference to the main IO-Context.
    asio::io_context& io_;
    /// A reference to the main SSL-Context.
    /// TODO: Dirty, change ot optional_ref
    ssl::context* ctx_;
    /// The TCP acceptor.
    tcp::acceptor acceptor_;
    /// The endpoint associated to the TCP acceptor.
    tcp::endpoint endpoint_;
    /// The shared state for each session.
    std::shared_ptr<SharedState> state_;

  private:
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
            LOG_ERROR("Accept error: {}\n", _error.message());
            return;
        }
        
        // Run the TCP / Http Listener and move ownership to it.
        if constexpr (std::is_same_v<Protocol, PlainHttpSession>) {
            std::make_shared<PlainHttpSession>(std::move(_socket), state_)
                ->run();
        } else if constexpr (std::is_same_v<Protocol, SSLHttpSession>) {
            std::make_shared<SSLHttpSession>(std::move(_socket), *ctx_, state_)
                ->run();
        } else if constexpr (std::is_same_v<Protocol, PlainTCPSession>) {
            std::make_shared<PlainTCPSession>(std::move(_socket), state_)
                ->run();
        } else if constexpr (std::is_same_v<Protocol, SSLTCPSession>) {
            std::make_shared<SSLTCPSession>(std::move(_socket), *ctx_, state_)
                ->run();
        }

        // Accept another connection.
        doAccept();
    }

  public:
    /// \brief Constructor for SSL Protocols.
    /// \param _io A reference to the main IO-Context.
    /// \param _ctx A reference to the main SSL-Context.
    /// \param _endpoint The endpoint to which the tcp acceptor will listen to.
    Listener(asio::io_context& _io, ssl::context& _ctx, tcp::endpoint _endpoint,
             std::shared_ptr<SharedState> const& _state)
        : io_(_io)
        , ctx_(&_ctx)
        , acceptor_(asio::make_strand(_io))
        , endpoint_(std::move(_endpoint))
        , state_(_state)
    {
    }

    /// \brief Constructor for Plain Protocols.
    /// \param _io A reference to the main IO-Context.
    /// \param _endpoint The endpoint to which the tcp acceptor will listen to.
    Listener(asio::io_context& _io, tcp::endpoint _endpoint,
             std::shared_ptr<SharedState> const& _state)
        : io_(_io)
        , ctx_(nullptr)
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