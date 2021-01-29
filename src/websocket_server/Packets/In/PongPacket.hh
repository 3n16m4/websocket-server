#ifndef WEBSOCKET_SERVER_OUT_PONG_PACKET_HH
#define WEBSOCKET_SERVER_OUT_PONG_PACKET_HH

namespace amadeus {
#pragma pack(push, 1)
namespace in {
/// \brief Defines the PongPacket which is received from a TCP Client.
struct PongPacket
{
    /// Packet header.
    std::uint8_t header;
};
#pragma pack(pop)
} // namespace in
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_PONG_PACKET_HH
