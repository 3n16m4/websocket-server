#include "websocket_server/Listener.hh"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/container/small_vector.hpp>

#include <iostream>
#include <string_view>
#include <thread>

using namespace amadeus;

struct CommandLineInterface
{
    /// The IP Address the server will listen to.
    asio::ip::address ip;
    /// The Port the server will listen to.
    std::uint16_t port;
    /// The amount of threads the server will utilize.
    std::uint32_t threads;

    void parse(int _argc, char* _argv[])
    {
        /// ./websocket_server <address> <port> <threads>
        if (_argc != 4) {
            throw std::invalid_argument(
                "Invalid arguments.\nUsage: ./websocket_server <address> "
                "<port> <threads>.\nExample: ./websocket_server 127.0.0.1 8080 "
                "8\nYou may also set <threads> to 0 to enable full utilization "
                "of the hardware threads.\n");
        }
        ip = asio::ip::make_address(_argv[1]);
        port = static_cast<decltype(port)>(std::atoi(_argv[2]));
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

    std::cout << "Starting server with " << cli.threads << " threads...\n";

    // The main io_context shared between all I/O operations and threads.
    asio::io_context io{static_cast<int>(cli.threads)};

    // Create and launch the listener.
    Listener listener{io, tcp::endpoint{cli.ip, cli.port}};
    try {
        listener.run();
    } catch (boost::system::system_error const& e) {
        std::cerr << e.what() << '\n';
        io.stop();
        return EXIT_FAILURE;
    }

    std::cout << "Server running at " << cli.ip << ":" << cli.port << "!\n";

    // Capture SIGINT, SIGTERM for a clean shutdown.
    asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(
        [&io](boost::system::error_code const& error, int signal_number) {
            // Stop the io_context and all of its associated handlers.
            io.stop();
            std::cout << "Exited with signal " << signal_number
                      << " and error: " << error.message() << '\n';
        });

    // Run the io_context for the specified amount of threads - 1.
    // This will make sure that at least one io_context is run on the
    // main-thread.
    auto constexpr TypicalMaxThreadCount{12U};
    boost::container::small_vector<std::thread, TypicalMaxThreadCount> threads;
    threads.reserve(cli.threads);
    for (auto i = cli.threads - 1; i > 0; --i) {
        threads.emplace_back([&io] { io.run(); });
    }

    io.run();

    // Wait for the threads to finish.
    for (auto&& t : threads) {
        t.join();
    }

    return EXIT_SUCCESS;
}