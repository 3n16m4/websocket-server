#ifndef WEBSOCKET_SERVER_WEBSOCKET_REQUEST_HANDLER_HH
#define WEBSOCKET_SERVER_WEBSOCKET_REQUEST_HANDLER_HH

#include "websocket_server/RequestHandler.hh"

#include <nlohmann/json.hpp>

#include <string_view>

using JSON = nlohmann::json;

namespace amadeus {
enum class RequestType
{
    WeatherStatus = 0x00,
    AvailableStations = 0x01,
};

template <class Session>
class WebSocketRequestHandler
{
  private:
    /// A reference to the WebSocketSession
    Session& session_;

  public:
    WebSocketRequestHandler(Session& _session)
        : session_(_session)
    {
    }

    HandlerReturnType handle(BufferView const _view)
    {
        auto const buffer = reinterpret_cast<std::uint8_t const*>(_view.data());
        auto const size = *reinterpret_cast<std::uint16_t const*>(buffer);

        LOG_INFO("Size = {}\n", size);
        LOG_INFO("Payload:\n");

        for (std::size_t i = 0; i < size; ++i) {
            fmt::print("{0:#04x} ", buffer[i]);
        }
        fmt::print("\n");

        // TODO: convert payload to JSON object
        // extract id
        // handle packet
        // return consumed bytes (size of payload)

        return std::make_pair(ResultType::Bad, 0);
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_WEBSOCKET_REQUEST_HANDLER_HH
