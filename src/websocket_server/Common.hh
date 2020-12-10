#ifndef WEBSOCKET_SERVER_COMMON_HH
#define WEBSOCKET_SERVER_COMMON_HH

#include <boost/config/helper_macros.hpp>

#include <chrono>

using namespace std::chrono_literals;

namespace amadeus {
/// The read / write / shutdown timeout for the asynchronous operations.
auto constexpr Timeout{60s};
/// The Handshake timeout for websockets.
auto constexpr HandshakeTimeout{30s};
/// The idle timeout for websockets.
auto constexpr IdleTimeout{10s};
/// The timeout for whenever a ping packet is sent.
auto constexpr PingTimeout{30s};
/// The timeout for whenever a pong packet is expected from the peer.
auto constexpr PongTimeout{5s};

/// Identifies the API version of the server.
#define SERVER_VERSION 10
#define SERVER_VERSION_STRING                                                  \
    "websocket-server/" BOOST_STRINGIZE(SERVER_VERSION)
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_COMMON_HH
