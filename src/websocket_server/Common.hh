#ifndef WEBSOCKET_SERVER_COMMON_HH
#define WEBSOCKET_SERVER_COMMON_HH

#include <chrono>

using namespace std::chrono_literals;

namespace amadeus {
/// The read / write / shutdown timeout for the asynchronous operations.
constexpr auto Timeout{30s};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_COMMON_HH