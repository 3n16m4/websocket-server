#ifndef WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH
#define WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH

namespace amadeus {
struct WeatherStatusPacket
{
    std::uint8_t header;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH