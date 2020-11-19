#ifndef WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH
#define WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH

namespace amadeus {
struct WeatherStatusPacket
{
    std::uint8_t header;
    float temperature;
    float humidity;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_IN_WEATHER_STATUS_PACKET_HH