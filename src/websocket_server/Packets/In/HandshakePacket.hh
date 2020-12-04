#ifndef WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH
#define WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH

namespace amadeus {
enum class StationId : std::uint8_t;
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
