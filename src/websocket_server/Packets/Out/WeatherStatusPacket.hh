#ifndef WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH
#define WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH

namespace amadeus {
#pragma pack(push, 1)
struct WeatherStatusPacket
{
    std::uint8_t header;
};
#pragma pack(pop)
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_WEATHER_STATUS_PACKET_HH