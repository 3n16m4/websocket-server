#ifndef WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH
#define WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH

#include <array>

namespace amadeus {
/// \brief Reserved, not used anymore.
enum class WebSocketSessionFlag : std::uint8_t
{
    Plain = 1 << 0,
    SSL = 1 << 1,
};

#pragma pack(push, 1)
namespace in {
/// \brief Defines the WeatherStatusPacket which is sent by the TCP Client to
/// notify the server about the temperature, humidity and the time the sensor
/// data were read.
struct WeatherStatusPacket
{
    /// Packet header.
    std::uint8_t header;
    /// A unique identifier for the corresponding TCP Client which is used for authentication.
    std::array<std::uint8_t, 16> uuid;
    /// The temperature ranging from 40°C - 100°C
    float temperature;
    /// The humidity in percentage ranging from 0% - 100%.
    float humidity;
    /// The time represented as a UNIX timestamp (epoch time since 01/01/1970).
    std::uint32_t time;
    /// A reserved server internal flag, not used anymore.
    WebSocketSessionFlag flag;
};
#pragma pack(pop)
} // namespace in
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH
