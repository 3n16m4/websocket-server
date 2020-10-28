#include "websocket_server/asiofwd.hh"

#include <boost/asio/io_context.hpp>

#include <iostream>
#include <string_view>

struct CommandLineInterface
{
    /// The IP Address the server will listen to.
    std::string_view ip;
    /// The Port the server will listen to.
    std::uint16_t port;

    void parse(int _argc, char* _argv[])
    {
        /// <address> <port> <threads>
        if (_argc != 4) {
            throw std::invalid_argument("Invalid arguments.\nUsage: ./websocket_server <address> <port> <threads>");
        }
        for (std::size_t i = 0; i < _argc; ++i) {
            std::string_view const arg{_argv[i]};
            
        }
    }
};

///
///    websocket-server:
///        -   No REST API, because HTTP Polling is not efficient.
///            -   HTTP Polling is slow, because the payload in each HTTP
///                Request is quite huge, compared to a simple WebSocket.
///        -   Instead, we'll implement a simple WebSocket Server.
///        -   The WebSockets are persistent and much faster and efficient than
///            HTTP Requests.
///        -   Fully asynchronous and multi-threaded.
///

/// \brief Example for Doxygen.
/// \param argc test
/// \param argv test2
int main(int argc, char* argv[])
{
    CommandLineInterface cli;
    try {
        cli.parse(argc, argv);
    } catch (std::exception const& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    asio::io_context io{1};

    io.run();
    return 0;
}