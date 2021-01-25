#ifndef WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH
#define WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH

#include <array>

namespace amadeus {
/// \brief Defines the available station ids.
enum class StationId : std::uint8_t
{
    Goe = 0,
    Wf = 1,
    Bs = 2,
    Max,
};

#pragma pack(push, 1)
namespace in {
/// \brief Defines the HandshakePacket which is received from a connected TPC Client.
struct HandshakePacket
{
    /// Packet header.
    std::uint8_t header{};
    /// A unique identifier for the corresponding TCP Client which is used for authentication.
    std::array<std::uint8_t, 16> uuid;
    /// Another unique identifier to map the TCP Client.
    StationId stationId;
};
#pragma pack(pop)
} // namespace in
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH
