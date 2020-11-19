#ifndef WEBSOCKET_SERVER_OUT_HH
#define WEBSOCKET_SERVER_OUT_HH

#include "websocket_server/Packets/Common.hh"
#include "websocket_server/Packets/Out/HandshakePacket.hh"

namespace amadeus {
namespace out {
enum class PacketType : std::uint8_t
{
    Handshake = 0x00,
    HandshakeACK = 0x01,
    HandshakeNAK = 0x02,
    Ping = 0x03,
    WeatherStatus = 0x04,
};
}
} // namespace amadeus


#endif // !WEBSOCKET_SERVER_OUT_HH