#ifndef WEBSOCKET_SERVER_IN_HH
#define WEBSOCKET_SERVER_IN_HH

#include "websocket_server/Packets/Common.hh"
#include "websocket_server/Packets/In/HandshakePacket.hh"

namespace amadeus {
namespace in {
enum class PacketType : std::uint8_t
{
    Handshake = 0x00,
    Pong = 0x01,
    WeatherStatus = 0x02,
};
}
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_IN_HH