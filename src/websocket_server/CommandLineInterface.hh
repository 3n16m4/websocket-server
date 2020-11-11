#ifndef WEBSOCKET_SERVER_COMMAND_LINE_INTERFACE_HH
#define WEBSOCKET_SERVER_COMMAND_LINE_INTERFACE_HH

#include "websocket_server/asiofwd.hh"

#include <boost/asio/ip/address.hpp>

#include <cstdint>
#include <string>

namespace amadeus {
/// \brief A simple CLI for parsing the necessary options for the TCP Listener.
struct CommandLineInterface
{
    /// The IP Address the server will listen to.
    asio::ip::address ip;
    /// The HTTP port the server will listen to.
    std::uint16_t httpPort;
    /// The TCP port the server will listen to.
    std::uint16_t tcpPort;
    /// The document root of all the resources the server will serve.
    std::string docRoot;
    /// The amount of threads the server will utilize.
    std::uint32_t threads;

    void parse(int _argc, char* _argv[]);
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_COMMAND_LINE_INTERFACE_HH