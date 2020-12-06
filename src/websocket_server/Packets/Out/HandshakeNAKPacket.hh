#ifndef WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH
#define WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH

namespace amadeus {
#pragma pack(push, 1)
enum class HandshakeReason : std::uint8_t
{
    // StationId was already assigned on the server.
    ReasonStationIdAlready = 1 << 0,
    // StationId is invalid because it could not be found.
    ReasonStationIdInvalid = 1 << 1,
    // Wrong UUID.
    ReasonUUIDInvalid = 1 << 2,
    // Wrong UUID format.
    ReasonUUIDInvalidFormat = 1 << 3,
};
namespace out {
struct HandshakeNAKPacket
{
    std::uint8_t header{0x02};
    HandshakeReason reason;
};
#pragma pack(pop)
} // namespace out
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH
