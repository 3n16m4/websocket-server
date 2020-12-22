#ifndef WEBSOCKET_SERVER_WEBSOCKET_REQUEST_HANDLER_HH
#define WEBSOCKET_SERVER_WEBSOCKET_REQUEST_HANDLER_HH

#include "websocket_server/RequestHandler.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/Packets/In/HandshakePacket.hh"

#include <nlohmann/json.hpp>
#include <magic_enum.hpp>

#include <string>

using JSON = nlohmann::json;

/// TODO: Should be indicate the session that it send too much data and just
/// discard the packet or just disconnect the session ?

namespace amadeus {
// in
enum class RequestType
{
    WeatherStatus = 0x00,
    AvailableStations = 0x01,
};

// out
enum class ResponseType
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
        auto constexpr MaxPayloadSize = 512U;

        if (_view.size() > MaxPayloadSize) {
            LOG_ERROR("Payload is too big to handle! Discarding packet...\n");
            return std::make_pair(ResultType::Bad, 0);
        }

        auto const buffer = reinterpret_cast<std::uint8_t const*>(_view.data());
        LOG_DEBUG("Complete buffer:\n");
        for (std::size_t i = 0; i < _view.size(); ++i) {
            fmt::print("{0:#04x} ", buffer[i]);
        }
        fmt::print("\n");

        auto const size = *reinterpret_cast<std::uint16_t const*>(buffer);
        auto const payload = buffer + 2;
        auto const totalSize = size + 2;

        LOG_DEBUG("Size = {}\n", size);
        LOG_DEBUG("TotalSize = {}\n", totalSize);
        LOG_DEBUG("Payload: ");
        for (std::size_t i = 0; i < size; ++i) {
            fmt::print("{0:#04x} ", payload[i]);
        }
        fmt::print("\n");

        std::string const payloadStr{payload, payload + size};

        try {
            auto json = JSON::parse(payloadStr);
            auto const requestId = json["id"].get<RequestType>();

            switch (requestId) {
            case RequestType::WeatherStatus:
                return handleWeatherStatusRequest(totalSize, std::move(json));
            case RequestType::AvailableStations:
                return handleAvailableStations(totalSize, std::move(json));
            }
        } catch (std::exception const& e) {
            LOG_ERROR("Failed to parse payload to JSON string: {}\n", e.what());
            return std::make_pair(ResultType::Bad, 0);
        }

        return std::make_pair(ResultType::Bad, 0);
    }

    HandlerReturnType handleWeatherStatusRequest(std::size_t _size, JSON _json)
    {
        LOG_DEBUG("WeatherStatusRequest JSON = {}\n", _json);

        // go through request
        // for each stationId in the request, try to obtain a shared_ptr for the
        // tcp session (can be both plain or ssl)
        // send weather status request

        return std::make_pair(ResultType::Good, _size);
    }

    HandlerReturnType handleAvailableStations(std::size_t _size, JSON _json)
    {
        LOG_DEBUG("AvailableStationsRequest JSON = {}\n", _json);

        auto& state = session_.sharedState();

        /// This is what the response could look like.
        /// {
        ///     "id": 1,
        ///     "stations": [
        ///         {
        ///             "stationId": 1,
        ///             "stationName": "WF"
        ///         },
        ///         {
        ///             "stationId": 2,
        ///             "stationName": "BS"
        ///         }
        ///     ]
        /// }
        ///
        /// Or if no stations are registered:
        ///
        /// {
        ///     "id": 1,
        ///     "stations": []
        /// }

        // go through each tcp session (both plain and ssl)
        // count sessions
        // write name and id into json response
        // send response to websocket session

        auto const stationIds = state.allStationIds();
        JSON response;
        response["id"] = ResponseType::AvailableStations;
        auto stations = JSON::array();
        for (auto const stationId : stationIds) {
            auto stationName =
                std::string{magic_enum::enum_name<StationId>(stationId)};

            auto&& entry = stations.emplace_back(JSON::object());
            entry["stationId"] = stationId;
            entry["stationName"] = std::move(stationName);
        }
        response["stations"] = std::move(stations);

        LOG_INFO("Response for AvailableStationsRequest = {}\n", response);

        session_.writeRequest(
            std::move(response), [this](auto&& bytes_transferred) {
                LOG_INFO("AvailableStationsRequest sent with {} bytes.\n",
                         bytes_transferred);
            });

        return std::make_pair(ResultType::Good, _size);
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_WEBSOCKET_REQUEST_HANDLER_HH
