#include "websocket_server/CommandLineInterface.hh"
#include "websocket_server/Logger.hh"

#include <fmt/format.h>

#include <stdexcept>
#include <thread>
#include <filesystem>
#include <fstream>
#include <string_view>

using namespace amadeus;

void CommandLineInterface::parse(int _argc, char* _argv[])
{
    if (_argc != 9) {
        throw std::invalid_argument(fmt::format(
            "Usage: {0} <address> <httpPort> <httpsPort> <tcpPort> "
            "<tcpSecurePort> <config> <threads>.\nExample: {0} 127.0.0.1 8080 "
            "8081 9090 9091 ../www ../config/config.json 8\nYou may also set "
            "<threads> to 0 to "
            "enable full "
            "utilization of the hardware threads.",
            _argv[0]));
    }
    ip = asio::ip::make_address(_argv[1]);
    httpPort = static_cast<decltype(httpPort)>(std::atoi(_argv[2]));
    httpsPort = static_cast<decltype(httpPort)>(std::atoi(_argv[3]));
    tcpPort = static_cast<decltype(tcpPort)>(std::atoi(_argv[4]));
    tcpSecurePort = static_cast<decltype(tcpPort)>(std::atoi(_argv[5]));
    docRoot = _argv[6];

    std::string_view const configFile{_argv[7]};

    std::ifstream file(configFile.data());
    if (!file.is_open()) {
        throw std::runtime_error(
            fmt::format("The specified file '{}' does not exist.", configFile));
    }

    try {
        config = JSON::parse(file);
        LOG_INFO("Config file '{}' successfully loaded with contents:\n{}\n",
                 configFile, config.dump(4));
    } catch (std::runtime_error const&) {
        throw;
    }

    threads = static_cast<decltype(threads)>(std::atoi(_argv[8]));

    if (threads == 0) {
        threads = std::thread::hardware_concurrency();
    }

    namespace fs = std::filesystem;
    if (!fs::exists(fs::path(docRoot))) {
        throw std::invalid_argument(fmt::format(
            "The given document root '{}' does not exist.", docRoot));
    }
}
