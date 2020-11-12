#include "websocket_server/CommandLineInterface.hh"
#include "websocket_server/HttpListener.hh"
#include "websocket_server/TCPListener.hh"
#include "websocket_server/ServerCertificate.hh"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/container/small_vector.hpp>

#include <iostream>
#include <thread>

using namespace amadeus;

/// \brief The main function.
/// \param argc Number of arguments.
/// \param argv The arguments.
int main(int argc, char* argv[])
{
    CommandLineInterface cli;
    try {
        cli.parse(argc, argv);
    } catch (std::invalid_argument const& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Starting server with " << cli.threads << " threads...\n";

    // The main io_context shared between all I/O operations and threads.
    asio::io_context io{static_cast<int>(cli.threads)};

    // The main SSL-Context shared between all SSL I/O operations and threads.
    // It holds the server certificate.
    ssl::context ctx{ssl::context::tls_server};

    /// Load self-signed certificate for the server.
    try {
        loadServerCertificate(ctx);
    } catch (boost::system::system_error const& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    // Create and launch the HTTP listener.
    HttpListener httpListener{io, ctx, tcp::endpoint{cli.ip, cli.httpPort},
                              std::make_shared<SharedState>(cli.docRoot)};

    // Create and launch the TCP listener.
    TCPListener tcpListener{io, ctx, tcp::endpoint{cli.ip, cli.tcpPort},
                            std::make_shared<SharedState>(cli.docRoot)};

    try {
        httpListener.run();
        tcpListener.run();
    } catch (boost::system::system_error const& e) {
        std::cerr << e.what() << '\n';
        io.stop();
        return EXIT_FAILURE;
    }

    std::cout << "Server running at " << cli.ip << ":" << cli.httpPort << "!\n";

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
    auto constexpr TypicalMaxThreadCount{8U};
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
