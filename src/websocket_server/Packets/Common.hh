#ifndef WEBSOCKET_SERVER_COMMON_HH
#define WEBSOCKET_SERVER_COMMON_HH

#include <array>
#include <cstdint>

// can be further extended...
enum class StationId : std::uint8_t
{
    GOE = 0,
    WF = 1,
    BS = 2,
};

enum class HandshakeReason : std::uint8_t
{
    /// The StationId was already assigned.
    StationIdAlready = 1 << 0,
    /// The UUID format is invalid.
    MalformedUUIDFormat = 1 << 1,
    /// The UUID is already registered.
    UUIDAlready = 1 << 2,
};

#endif // !WEBSOCKET_SERVER_COMMON_HH