#ifndef WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH
#define WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH

namespace amadeus {
enum class StationId;
#pragma pack(push, 1)
struct HandshakePacket
{
    std::uint8_t header{};
    std::array<std::uint8_t, 16> uuid;
    StationId stationId;
};
#pragma pack(pop)
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_IN_HANDSHAKE_PACKET_HH