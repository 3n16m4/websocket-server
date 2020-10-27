#include <boost/asio/io_context.hpp>

#include <iostream>
#include <string_view>

namespace asio = boost::asio;

struct CLI
{
    void parse(int argc, char* argv[])
    {
        for (std::size_t i = 0; i < argc; ++i) {
            std::string_view const arg{argv[i]};
            std::cout << arg << '\n';
        }
    }
};

///
///    muttp:
///        -   No REST API, because HTTP Polling is not efficient.
///            -   HTTP Polling is slow, because the payload in each HTTP
///                Request is quite huge, compared to a simple WebSocket.
///        -   Instead, we'll implement a simple WebSocket Server.
///        -   The WebSockets are persistent and much faster and efficient than
///            HTTP Requests.
///        -   Fully asynchronous and multi-threaded.
///
int main(int argc, char* argv[])
{
    CLI cli;
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