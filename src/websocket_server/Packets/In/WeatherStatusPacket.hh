#ifndef WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH
#define WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH

#include <array>

namespace amadeus {
enum class WebSocketSessionFlag : std::uint8_t
{
    Plain = 1 << 0,
    SSL = 1 << 1,
};

#pragma pack(push, 1)
namespace in {
struct WeatherStatusPacket
{
    std::uint8_t header;
    std::array<std::uint8_t, 16> uuid;
    float temperature;
    float humidity;
    WebSocketSessionFlag flag;
};
#pragma pack(pop)
} // namespace in
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH
