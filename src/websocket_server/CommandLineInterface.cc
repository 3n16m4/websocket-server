#include "websocket_server/CommandLineInterface.hh"

#include <iostream>
#include <stdexcept>
#include <thread>

using namespace amadeus;

void CommandLineInterface::parse(int _argc, char* _argv[])
{
    // ./websocket_server <address> <port> <docroot> <threads>
    if (_argc != 5) {
        throw std::invalid_argument(
            "Usage: ./websocket_server <address> "
            "<port> <threads>.\nExample: ./websocket_server 127.0.0.1 8080 "
            ". "
            "8\nYou may also set <threads> to 0 to enable full utilization "
            "of the hardware threads.");
    }
    ip = asio::ip::make_address(_argv[1]);
    port = static_cast<decltype(port)>(std::atoi(_argv[2]));
    docRoot = _argv[3];
    threads = static_cast<decltype(threads)>(std::atoi(_argv[4]));

    if (threads == 0) {
        threads = std::thread::hardware_concurrency();
    }

    std::cout << "Arguments: " << ip << " " << port << " " << docRoot << " "
              << threads << '\n';
}
