#include "websocket_server/CommandLineInterface.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/PlainHttpListener.hh"
#include "websocket_server/PlainTCPListener.hh"
#include "websocket_server/SSLHttpListener.hh"
#include "websocket_server/SSLTCPListener.hh"
#include "websocket_server/ServerCertificate.hh"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/container/small_vector.hpp>

#include <ratio>
#include <thread>

using namespace amadeus;

/// \brief The main function.
/// \param argc Number of arguments.
/// \param argv The arguments.
int main(int argc, char* argv[])
{
    auto& logger = Logger::instance();
    logger.open("server_log.txt");

    CommandLineInterface cli;
    try {
        cli.parse(argc, argv);
    } catch (std::exception const& e) {
        LOG_ERROR("{}\n", e.what());
        return EXIT_FAILURE;
    }

    LOG_INFO("Starting server with {} threads...\n", cli.threads);

    // The main io_context shared between all I/O operations and threads.
    asio::io_context io{static_cast<int>(cli.threads)};

    // The main SSL-Context shared between all SSL I/O operations and threads.
    // It holds the server certificate.
    ssl::context ctx{ssl::context::tls_server};

    /// Load self-signed certificate for the server.
    try {
        loadServerCertificate(ctx);
    } catch (boost::system::system_error const& e) {
        LOG_FATAL("{}\n", e.what());
        return EXIT_FAILURE;
    }

    // Create and launch the HTTP listeners.
    PlainHttpListener plainHttpListener{
        io, tcp::endpoint{cli.ip, cli.httpPort},
        std::make_shared<SharedState>(std::move(cli.docRoot), cli.config)};

    SSLHttpListener sslHttpListener{
        io, ctx, tcp::endpoint{cli.ip, cli.httpsPort},
        std::make_shared<SharedState>(std::move(cli.docRoot), cli.config)};

    // Create and launch the TCP listeners.
    PlainTCPListener plainTCPListener{
        io, tcp::endpoint{cli.ip, cli.tcpPort},
        std::make_shared<SharedState>(std::move(cli.docRoot), cli.config)};

    SSLTCPListener sslTCPListener{
        io, ctx, tcp::endpoint{cli.ip, cli.tcpSecurePort},
        std::make_shared<SharedState>(std::move(cli.docRoot), cli.config)};

    try {
        plainHttpListener.run();
        sslHttpListener.run();
        plainTCPListener.run();
        sslTCPListener.run();
    } catch (boost::system::system_error const& e) {
        LOG_FATAL("{}\n", e.what());
        io.stop();
        return EXIT_FAILURE;
    }

    LOG_INFO("Server running at {}:{}\n", cli.ip, cli.httpPort);

    // Capture SIGINT, SIGTERM for a clean shutdown.
    asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(
        [&io](boost::system::error_code const& error, int signal_number) {
            // Stop the io_context and all of its associated handlers.
            auto const start = std::chrono::steady_clock::now();
            io.stop();
            auto const diff = std::chrono::duration<double, std::milli>(
                                  std::chrono::steady_clock::now() - start)
                                  .count();
            LOG_INFO("Exited with signal {} and error: {}. Took {} ms.\n",
                     signal_number, error.message(), diff);
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
