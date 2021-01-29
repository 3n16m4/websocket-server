#ifndef WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH
#define WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH

#include <array>

namespace amadeus {
enum class WebSocketSessionFlag : std::uint8_t;
#pragma pack(push, 1)
namespace out {
/// \brief Defines the WeatherStatusPacket which is sent by the server if a
/// WebSocketSession requested a weather update for a specific TCP connection
/// (µC).
struct WeatherStatusPacket
{
    /// Packet header.
    std::uint8_t header{0x04};
    /// A unique identifier for the corresponding TCP Client which is used for
    /// authentication.
    std::array<std::uint8_t, 16> uuid;
    /// Reserved server specific flag, not used anymore.
    WebSocketSessionFlag flag;
};
#pragma pack(pop)
} // namespace out
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH
