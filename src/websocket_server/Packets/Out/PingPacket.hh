#ifndef WEBSOCKET_SERVER_OUT_PING_PACKET_HH
#define WEBSOCKET_SERVER_OUT_PING_PACKET_HH

namespace amadeus {
struct PingPacket
{
    std::uint8_t header;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_PING_PACKET_HH