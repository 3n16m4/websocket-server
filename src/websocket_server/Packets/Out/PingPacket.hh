#ifndef WEBSOCKET_SERVER_OUT_PING_PACKET_HH
#define WEBSOCKET_SERVER_OUT_PING_PACKET_HH

namespace amadeus {
#pragma pack(push, 1)
namespace out {
/// \brief Defines the PingPacket which is sent by the server every 30 seconds
/// to the TCP Client. This mechanism (ping - pong) ensures that both sides are
/// still connected and thus acts as a heartbeat between the two endpoints.
struct PingPacket
{
    /// Packet header.
    std::uint8_t header{0x03};
};
#pragma pack(pop)
} // namespace out
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_PING_PACKET_HH
