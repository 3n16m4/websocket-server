#ifndef WEBSOCKET_SERVER_OUT_HANDSHAKE_ACK_PACKET_HH
#define WEBSOCKET_SERVER_OUT_HANDSHAKE_ACK_PACKET_HH

namespace amadeus {
#pragma pack(push, 1)
namespace out {
struct HandshakeACKPacket
{
    std::uint8_t header{0x01};
};
#pragma pack(pop)
} // namespace out
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_HANDSHAKE_ACK_PACKET_HH
