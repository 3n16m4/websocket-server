#include "websocket_server/asiofwd.hh"

#include <boost/asio/io_context.hpp>

#include <iostream>
#include <string_view>
#include <thread>

struct CommandLineInterface
{
    /// The IP Address the server will listen to.
    std::string_view ip;
    /// The Port the server will listen to.
    std::string_view port;
    /// The amount of threads the server will utilize.
    std::uint32_t threads;

    void parse(int _argc, char* _argv[])
    {
        /// <app> <address> <port> <threads>
        if (_argc != 4) {
            throw std::invalid_argument(
                "Invalid arguments.\nUsage: ./websocket_server <address> "
                "<port> <threads>.\nExample: ./websocket_server 127.0.0.1 8080 "
                "8\nYou may also set <threads> to 0 to enable full utilization "
                "of the hardware threads.\n");
        }
        ip = _argv[1];
        port = _argv[2];
        threads = static_cast<decltype(threads)>(std::atoi(_argv[3]));

        if (threads == 0) {
            threads = std::thread::hardware_concurrency();
        }

        std::cout << "Arguments: " << ip << " " << port << " " << threads
                  << '\n';
    }
};

/// \brief The main function.
/// \param argc Number of arguments.
/// \param argv The arguments.
int main(int argc, char* argv[])
{
    CommandLineInterface cli;
    try {
        cli.parse(argc, argv);
    } catch (std::exception const& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    asio::io_context io{static_cast<int>(cli.threads)};

    io.run();
    return 0;
}