#ifndef WEBSOCKET_SERVER_OUT_HANDSHAKE_PACKET_HH
#define WEBSOCKET_SERVER_OUT_HANDSHAKE_PACKET_HH

namespace amadeus {
#pragma pack(push, 1)
namespace out {
struct HandshakePacket
{
    std::uint8_t header{0x00};
};
#pragma pack(pop)
} // namespace out
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_HANDSHAKE_PACKET_HH
