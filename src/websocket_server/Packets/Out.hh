#ifndef WEBSOCKET_SERVER_OUT_HH
#define WEBSOCKET_SERVER_OUT_HH

#include "websocket_server/Packets/Common.hh"
#include "websocket_server/Packets/Out/HandshakeACKPacket.hh"
#include "websocket_server/Packets/Out/HandshakeNAKPacket.hh"
#include "websocket_server/Packets/Out/HandshakePacket.hh"
#include "websocket_server/Packets/Out/PingPacket.hh"
#include "websocket_server/Packets/Out/WeatherStatusPacket.hh"

namespace amadeus {
namespace out {
/// \brief Defines the PacketType enum which represents all incoming packets
/// from the TCP connections.
enum class PacketType : std::uint8_t
{
    Handshake = 0x00,
    HandshakeACK = 0x01,
    HandshakeNAK = 0x02,
    Ping = 0x03,
    WeatherStatus = 0x04,
};
} // namespace out
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_HH
