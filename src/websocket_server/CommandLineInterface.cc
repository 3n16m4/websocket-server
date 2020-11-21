#include "websocket_server/CommandLineInterface.hh"

#include <fmt/format.h>

#include <stdexcept>
#include <thread>
#include <filesystem>

using namespace amadeus;

/// TODO: Update example.
void CommandLineInterface::parse(int _argc, char* _argv[])
{
    // ./websocket_server <address> <httpPort> <httpsPort> <tcpPort>
    // <tcpSecurePort> <docroot> <threads>
    if (_argc != 8) {
        throw std::invalid_argument(fmt::format(
            "Usage: {0} <address> "
            "<port> <threads>.\nExample: {0} 127.0.0.1 8080 "
            "8081 /www 8\nYou may also set <threads> to 0 to enable full "
            "utilization of the hardware threads.",
            _argv[0]));
    }
    ip = asio::ip::make_address(_argv[1]);
    httpPort = static_cast<decltype(httpPort)>(std::atoi(_argv[2]));
    httpsPort = static_cast<decltype(httpPort)>(std::atoi(_argv[3]));
    tcpPort = static_cast<decltype(tcpPort)>(std::atoi(_argv[4]));
    tcpSecurePort = static_cast<decltype(tcpPort)>(std::atoi(_argv[5]));
    docRoot = _argv[6];
    threads = static_cast<decltype(threads)>(std::atoi(_argv[7]));

    if (threads == 0) {
        threads = std::thread::hardware_concurrency();
    }

    namespace fs = std::filesystem;
    if (!fs::exists(fs::path(docRoot))) {
        throw std::invalid_argument("The given document root '" + docRoot +
                                    "' does not exist.");
    }
}
