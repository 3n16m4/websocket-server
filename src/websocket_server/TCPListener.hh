#ifndef WEBSOCKET_SERVER_LISTENER_HH
#define WEBSOCKET_SERVER_LISTENER_HH

#include "websocket_server/asiofwd.hh"

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>

namespace amadeus {
/// \brief This class is responsible for accepting incoming connections. It is
/// important to note, that this class does not 'manage' any newly created
/// connection by any means. In fact, the connection itself is responsible for
/// its own lifetime management. This is done by creating shared pointers for
/// each connection and making the connection itself inherit from
/// enable_shared_from_this.
class TCPListener
{
  private:
    /// A reference to the main IO-Context.
    asio::io_context& io_;
    /// The TCP acceptor.
    tcp::acceptor acceptor_;
    /// The endpoint associated to the TCP acceptor.
    tcp::endpoint endpoint_;

    /// \brief Called each time a new connection was accepted.
    void onAccept(beast::error_code const& _error, tcp::socket&& _socket);

  public:
    /// \brief Constructor.
    /// \param _io A reference to the main IO-Context.
    /// \param _endpoint The endpoint to which the tcp acceptor will listen to.
    TCPListener(asio::io_context& _io, tcp::endpoint _endpoint);

    /// \brief Starts the listener and accepting incoming connections.
    /// \throw Any exception thrown by boost.
    void run();
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_LISTENER_HH