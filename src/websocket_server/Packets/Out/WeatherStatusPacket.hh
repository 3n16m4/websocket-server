#ifndef WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH
#define WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH

#include <array>

namespace amadeus {
enum class WebSocketSessionFlag : std::uint8_t;
#pragma pack(push, 1)
namespace out {
struct WeatherStatusPacket
{
    std::uint8_t header{0x04};
    std::array<std::uint8_t, 16> uuid;
    WebSocketSessionFlag flag;
};
#pragma pack(pop)
} // namespace out
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH
