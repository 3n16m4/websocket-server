#ifndef WEBSOCKET_SERVER_TCP_PACKET_HANDLER_HH
#define WEBSOCKET_SERVER_TCP_PACKET_HANDLER_HH

#include "websocket_server/RequestHandler.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/Packets/In.hh"
#include "websocket_server/Packets/Out.hh"
#include "websocket_server/SharedState.hh"
#include "websocket_server/utils/packet_view.hh"

#include <boost/asio/steady_timer.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

/// \brief The TCPRequestHandler for incoming µc-TCP requests.
/// A TCPRequestHandler can vary in its design and underlying data structure
/// choice which heavily depends on the network partner (server).
/// Each TCPRequestHandler implementation however, must override the handle
/// method and call its specific handlers inside it.
namespace amadeus {
namespace in {
enum class PacketType : std::uint8_t;
} // namespace in

template <class Session>
class TCPRequestHandler
{
  public:
    /// The underlying PacketId Type.
    using PacketIdType = std::underlying_type<in::PacketType>::type;

    /// \brief Constructor.
    TCPRequestHandler(asio::io_context& _ioc, Session& _session)
        : session_(_session)
        , pingTimer_(_ioc, PingTimeout)
        , pongTimer_(_ioc, PongTimeout)
    {
    }

    /// \brief Parse some data. The enum return value is 'Good' when a
    /// complete request has been parsed, 'Bad' if the data is invalid,
    /// 'Indeterminate' when more data is required. The std::size_t return
    /// value indicates how much of the input has been consumed. See \ref
    /// HandlerReturnType for more details.
    HandlerReturnType handle(in::PacketType _id, BufferView const _view)
    {
        using namespace in;
        auto const expectedSize = sizeByPacketId(_id);

        // If it is not big enough, we'll simply return 'Indeterminate' to the
        // caller to notify that it should read more into the input buffer
        // (append after the currently read bytes).
        if (_view.size() < expectedSize) {
            return std::make_pair(ResultType::Indeterminate, 0);
        }

        // Otherwise we'll simply invoke the corresponding handler and return
        // 'Good' if the handler was able to parse the data correctly. (At this
        // point, the handler can fully trust that _view.size() is big enough to
        // have a view of its packet.)

        LOG_DEBUG("PacketId: {0:#04x}\n", _id);

        switch (_id) {
        case PacketType::Handshake:
            return handleHandshakePacket(_view);
        case PacketType::Pong:
            return handlePongPacket(_view);
        case PacketType::WeatherStatus:
            return handleWeatherStatusPacket(_view);
        default:
            LOG_ERROR(
                "Unable to find handler callback for PacketId '{0:#04x}'.\n",
                _id);
        }

        return std::make_pair(ResultType::Bad, 0);
    }

    void stop()
    {
        pingTimer_.cancel();
        pongTimer_.cancel();

        // auto& session = session_.derived();
        // session.disconnect();
    }

  private:
    /// A reference to the TCPSession.
    Session& session_;
    /// If no pong is received from the peer for a time equal to this
    /// timeout, then the connection will be shut down.
    asio::steady_timer pongTimer_;
    /// Whenever this timeout expires, a ping packet will be sent to the peer.
    asio::steady_timer pingTimer_;

    HandlerReturnType handleHandshakePacket(BufferView const _view)
    {
        packet_view<in::HandshakePacket> const packet{_view.data()};

        LOG_INFO("handleHandshakePacket called with view: {}.\n", packet);

        auto sendBadRequest = [&, this](auto&& _error) -> HandlerReturnType {
            out::HandshakeNAKPacket handshakeNAK{};
            handshakeNAK.reason = _error;

            session_.writePacket(
                handshakeNAK, [this](auto&& bytes_transferred) {
                    LOG_INFO("HandshakeNAKPacket sent with {} bytes.\n",
                             bytes_transferred);
                });

            return std::make_pair(ResultType::Good, packet.size());
        };

        // check if the stationId is valid.
        using enum_type = std::underlying_type<StationId>::type;

        HandshakeReason error{};

        // Is the id in range?
        auto const id = static_cast<enum_type>(packet->stationId);
        if (!(id >= 0 && id < static_cast<enum_type>(StationId::Max))) {
            LOG_DEBUG("StationId is not in range.\n");
            return sendBadRequest(HandshakeReason::ReasonStationIdInvalid);
        }

        boost::uuids::uuid uuid;
        std::memcpy(&uuid, packet->uuid.data(), packet->uuid.size());

        auto const isValidUUID = [this](std::string const& maybe_uuid) {
            using namespace boost::uuids;

            try {
                auto const result = string_generator()(maybe_uuid);
                return result.version() != uuid::version_unknown;
            } catch (std::runtime_error const&) {
                return false;
            }
        };

        auto const uuidStr = boost::uuids::to_string(uuid);
        if (!isValidUUID(uuidStr)) {
            LOG_DEBUG("UUID format is invalid.\n");
            return sendBadRequest(HandshakeReason::ReasonUUIDInvalidFormat);
        }

        /// \brief Returns true if the given UUID is registered in the config,
        /// false otherwise.
        auto const isUUIDRegistered =
            [this](std::string_view _uuid) noexcept -> bool {
            auto const& config = session_.sharedState().config();
            for (auto const& e : config["uuids"]) {
                auto const uuidView =
                    e["uuid"].template get<std::string_view>();
                if (_uuid == uuidView) {
                    return true;
                }
            }
            return false;
        };

        LOG_DEBUG("UUID from packet = {0:#04x}\n",
                  fmt::join(packet->uuid, ", "));

        if (isUUIDRegistered(uuidStr.c_str())) {
            auto const joined = session_.sharedState().join(
                packet->stationId, &session_.derived());

            if (!joined) {
                LOG_DEBUG("StationId already exists.\n");
                return sendBadRequest(HandshakeReason::ReasonStationIdAlready);
            }

            // save the StationId for this session.
            session_.stationId(packet->stationId);

            startPingTimer();

            out::HandshakeACKPacket const handshakeACK{};
            session_.writePacket(
                handshakeACK, [this](auto&& bytes_transferred) {
                    LOG_INFO("HandshakeACKPacket sent with {} bytes.\n",
                             bytes_transferred);
                });
        } else {
            LOG_DEBUG("UUID is not registered for this TCPSession.\n");
            return sendBadRequest(HandshakeReason::ReasonUUIDInvalid);
        }

        return std::make_pair(ResultType::Good, packet.size());
    }

    HandlerReturnType handlePongPacket(BufferView const _view)
    {
        packet_view<in::PongPacket> const packet{_view.data()};

        LOG_INFO("handlePongPacket called with view: {}.\n", packet);

        pongTimer_.cancel();

        return std::make_pair(ResultType::Good, packet.size());
    }

    HandlerReturnType handleWeatherStatusPacket(BufferView const _view) const
    {
        packet_view<in::WeatherStatusPacket> const packet{_view.data()};

        LOG_INFO("handleWeatherStatusPacket called with view: {}\n", packet);

        LOG_INFO("Temperature: {} Humidity: {}\n", packet->temperature,
                 packet->humidity);

        // convert uuid from packet to boost::uuids::uuid
        boost::uuids::uuid uuid;
        std::memcpy(&uuid, packet->uuid.data(), uuid.size());

        auto& state = session_.sharedState();

        WeatherStatusNotification notification;
        notification.id = session_.stationId();
        notification.temperature = packet->temperature;
        notification.humidity = packet->humidity;
        notification.time = packet->time;

        /// notify webscket session about packet!
        if (packet->flag == WebSocketSessionFlag::Plain) {
            /// TODO: Refactor me!
            // obtain PlainWebSocketSession shared_ptr and call callback
            auto callback =
                state.template findWebSocketSession<PlainWebSocketSession>(
                    uuid);
            if (callback) {
                callback(notification);
            } else {
                LOG_ERROR("PlainWebSocketSession not found by UUID!!!\n");
            }
        } else if (packet->flag == WebSocketSessionFlag::SSL) {
            // obtain SSLWebSocketSession shared_ptr and call callback
            auto callback =
                state.template findWebSocketSession<SSLWebSocketSession>(uuid);
            if (callback) {
                callback(notification);
            } else {
                LOG_ERROR("SSLWebSocketSession not found by UUID!!!\n");
            }
        }

        return std::make_pair(ResultType::Good, packet.size());
    }

    void startPingTimer()
    {
        pingTimer_.async_wait(
            [this, self = session_.derived().shared_from_this()](
                auto&& _error) { onPingTimeout(_error); });
    }

    void startPongTimer()
    {
        pongTimer_.expires_after(PongTimeout);
        pongTimer_.async_wait(
            [this, self = session_.derived().shared_from_this()](
                auto&& _error) { onPongTimeout(_error); });
    }

    void restartPingTimer()
    {
        pingTimer_.expires_at(pingTimer_.expiry() + PingTimeout);
        startPingTimer();
    }

    void restartPongTimer()
    {
        pongTimer_.expires_at(pongTimer_.expiry() + PongTimeout);
        startPongTimer();
    }

    void onPingTimeout(boost::system::error_code const& _error)
    {
        if (_error == asio::error::operation_aborted) {
            return;
        }

        out::PingPacket const packet{};
        session_.writePacket(packet, [this](auto&& bytes_transferred) {
            LOG_INFO("PingPacket sent with {} bytes.\n", bytes_transferred);
            startPongTimer();
        });

        restartPingTimer();
    }

    void onPongTimeout(boost::system::error_code const& _error)
    {
        if (_error == asio::error::operation_aborted) {
            return;
        }

        LOG_ERROR("We haven't received a pong packet in the right time! "
                  "disconnecting peer...\n");

        auto& session = session_.derived();
        session.disconnect();
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_TCP_PACKET_HANDLER_HH
