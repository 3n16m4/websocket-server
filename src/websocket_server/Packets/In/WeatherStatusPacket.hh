#ifndef WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH
#define WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH

namespace amadeus {
#pragma pack(push, 1)
namespace in {
struct WeatherStatusPacket
{
    std::uint8_t header;
    float temperature;
    float humidity;
};
#pragma pack(pop)
} // namespace in
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH
