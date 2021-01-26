#ifndef WEBSOCKET_SERVER_WEBSOCKET_REQUEST_HANDLER_HH
#define WEBSOCKET_SERVER_WEBSOCKET_REQUEST_HANDLER_HH

#include "websocket_server/RequestHandler.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/Packets/In/HandshakePacket.hh"
#include "websocket_server/Packets/Out/WeatherStatusPacket.hh"
#include "websocket_server/SharedState.hh"
#include "websocket_server/PlainTCPSession.hh"
#include "websocket_server/SSLTCPSession.hh"

#include <nlohmann/json.hpp>
#include <magic_enum.hpp>

#include <string>

using JSON = nlohmann::json;

/// TODO: Should we indicate the session that it send too much data and just
/// discard the packet or just disconnect the session ?

namespace amadeus {
// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/// \brief Defines the RequestType enum which includes the incoming WebSocket
/// requests.
enum class RequestType
{
    WeatherStatus = 0x00,
    AvailableStations = 0x01,
};

/// \brief Defines the ResponseType enum which includes the outgoing WebSocket
/// responses.
enum class ResponseType
{
    WeatherStatus = 0x00,
    AvailableStations = 0x01,
};

/// \brief Similar to the \ref TCPRequestHandler, this request handler is
/// responsible for handling the JSON requests from the WebSocket session.
template <class Session>
class WebSocketRequestHandler
{
  private:
    /// A reference to the WebSocketSession
    Session& session_;

  public:
    /// \brief Creates a new WebSocketRequestHandler.
    /// \param _session A reference to the Session.
    WebSocketRequestHandler(Session& _session)
        : session_(_session)
    {
    }

    /// \brief Parse some data. The enum return value is 'Good' when a
    /// complete request has been parsed, 'Bad' if the data is invalid,
    /// 'Indeterminate' when more data is required. The std::size_t return
    /// value indicates how much of the input has been consumed. See \ref
    /// HandlerReturnType for more details.
    /// This is very similar to the handle function from \ref TCPRequestHandler.
    /// However, as we receive JSON data, we need to parse the packet
    /// differently.
    /// Each JSON packet contains a 2 byte big endian frame at the beginning of
    /// the frame. This marks the size of the actual payload which directly sits
    /// after the 2 bytes. As such, we can simply parse the first 2 bytes,
    /// extract the size and read the actual payload which is offsetted by 2
    /// bytes.
    /// Example: 04 00 01 02 03 04
    /// Size:    04 00 --> 04 --> 4 bytes
    /// Payload: 01 02 03 04
    HandlerReturnType handle(BufferView const _view)
    {
        auto constexpr MaxPayloadSize = 512U;

        if (_view.size() > MaxPayloadSize) {
            LOG_ERROR("Payload is too big to handle! Discarding packet...\n");
            return std::make_pair(ResultType::PayloadTooBig, 0);
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

    /// \brief Handler function for the incoming WeatherStatusRequest from the
    /// WebSocket connection.
    /// \param _size The size of the JSON payload.
    /// \param _json The entire JSON payload.
    HandlerReturnType handleWeatherStatusRequest(std::size_t _size, JSON _json)
    {
        LOG_DEBUG("WeatherStatusRequest JSON = {}\n", _json);

        // go through request
        if (!_json.contains("stationIds")) {
            return std::make_pair(ResultType::Bad, _size);
        }

        auto const stations = _json["stationIds"];
        LOG_DEBUG("Num Requested stations: {}\n", stations.size());

        auto& state = session_.sharedState();
        // for each stationId in the request, try to obtain a shared_ptr for the
        // tcp session (can be both plain or ssl)
        for (auto const id : stations) {
            SharedState::VariantType sp;

            // prepare payload
            out::WeatherStatusPacket packet{};
            std::memcpy(packet.uuid.data(), &session_.uuid(),
                        packet.uuid.size());

            sp = state.findStation(id);
            std::visit(
                overloaded{
                    [&](std::shared_ptr<PlainTCPSession> const& ptr) {
                        if (ptr) {
                            LOG_DEBUG("setting flag to Plain\n");
                            packet.flag = WebSocketSessionFlag::Plain;
                            LOG_DEBUG("shared_ptr<PlainTCPSession> found for "
                                      "session with id {}!\n",
                                      id);
                            // send weather request to µc
                            ptr->writePacket(
                                packet, [this](auto&& bytes_transferred) {
                                    LOG_INFO("WeatherStatusRequest sent with "
                                             "{} bytes.\n",
                                             bytes_transferred);
                                });
                        }
                    },
                    [&](std::shared_ptr<SSLTCPSession> const& ptr) {
                        if (ptr) {
                            LOG_DEBUG("setting flag to SSL\n");
                            packet.flag = WebSocketSessionFlag::SSL;
                            LOG_DEBUG("shared_ptr<SSLTCPSession> found for "
                                      "session with id {}!\n",
                                      id);
                            // send weather request to µc
                            ptr->writePacket(
                                packet, [this](auto&& bytes_transferred) {
                                    LOG_INFO("WeatherStatusRequest sent with "
                                             "{} bytes.\n",
                                             bytes_transferred);
                                });
                        }
                    },
                    [&](std::monostate) { LOG_ERROR("No StationId found!\n"); },
                },
                sp);
        }

        return std::make_pair(ResultType::Good, _size);
    }

    /// \brief Handler function for the incoming AvailableStationsRequest from
    /// the WebSocket connection.
    /// \param _size The size of the JSON payload.
    /// \param _json The entire JSON payload.
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
