#ifndef WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH
#define WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH

namespace amadeus {
enum class HandshakeReason;
struct HandshakeNAKPacket
{
    std::uint8_t header;
    HandshakeReason reason;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH