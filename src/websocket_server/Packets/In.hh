#ifndef WEBSOCKET_SERVER_IN_HH
#define WEBSOCKET_SERVER_IN_HH

#include "websocket_server/Packets/Common.hh"
#include "websocket_server/Packets/In/HandshakePacket.hh"
#include "websocket_server/Packets/In/PongPacket.hh"
#include "websocket_server/Packets/In/WeatherStatusPacket.hh"

#include <string_view>

namespace amadeus {
namespace in {
enum class PacketType : std::uint8_t
{
    Handshake = 0x00,
    Pong = 0x01,
    WeatherStatus = 0x02,
};

/// \brief Returns the exact size of the packet by a given valid PacketId.
constexpr std::size_t sizeByPacketId(PacketType _id) noexcept
{
    switch (_id) {
    case PacketType::Handshake:
        return sizeof(HandshakePacket);
    case PacketType::Pong:
        return sizeof(PongPacket);
    case PacketType::WeatherStatus:
        return sizeof(WeatherStatusPacket);
    default:
        break;
    }
    return 0;
}

constexpr std::string_view packetNameById(PacketType _id) noexcept
{
    switch (_id) {
    case PacketType::Handshake:
        return "Handshake";
    case PacketType::Pong:
        return "Pong";
    case PacketType::WeatherStatus:
        return "WeatherStatus";
    }
    return "unknown";
}
} // namespace in
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_IN_HH
