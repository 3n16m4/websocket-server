#ifndef WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH
#define WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH

namespace amadeus {
enum class HandshakeReason : std::uint8_t;
#pragma pack(push, 1)
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
