#ifndef WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH
#define WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH

#include <array>

namespace amadeus {
/// TODO: Maybe extend to something for meaningful such as "LivingRoom",
/// "Bathroom", "Garden", etc.
enum class StationId : std::uint8_t
{
    Goe = 0,
    Wf = 1,
    Bs = 2,
    Max,
};

#pragma pack(push, 1)
namespace in {
struct HandshakePacket
{
    std::uint8_t header{};
    std::array<std::uint8_t, 16> uuid;
    StationId stationId;
};
#pragma pack(pop)
} // namespace in
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH
