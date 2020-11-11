#include "websocket_server/CommandLineInterface.hh"

#include <iostream>
#include <stdexcept>
#include <thread>

using namespace amadeus;

void CommandLineInterface::parse(int _argc, char* _argv[])
{
    // ./websocket_server <address> <httpPort> <tcpPort> <docroot> <threads>
    if (_argc != 6) {
        throw std::invalid_argument(
            "Usage: ./websocket_server <address> "
            "<port> <threads>.\nExample: ./websocket_server 127.0.0.1 8080 "
            "8081 /www 8\nYou may also set <threads> to 0 to enable full "
            "utilization of the hardware threads.");
    }
    ip = asio::ip::make_address(_argv[1]);
    httpPort = static_cast<decltype(httpPort)>(std::atoi(_argv[2]));
    tcpPort = static_cast<decltype(tcpPort)>(std::atoi(_argv[3]));
    docRoot = _argv[4];
    threads = static_cast<decltype(threads)>(std::atoi(_argv[5]));

    if (threads == 0) {
        threads = std::thread::hardware_concurrency();
    }

    std::cout << "Arguments: " << ip << " " << httpPort << " " << docRoot << " "
              << threads << '\n';
}
