#ifndef WEBSOCKET_SERVER_REQUEST_HANDLER_HH
#define WEBSOCKET_SERVER_REQUEST_HANDLER_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/Packets/In.hh"
#include "websocket_server/Packets/Out.hh"
#include "websocket_server/utils/packet_view.hh"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/buffer.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

/// \brief The PacketHandler for incoming µc-TCP requests.
/// A PacketHandler can vary in its design and underlying data structure
/// choice which heavily depends on the network partner (server).
/// Each PacketHandler implementation however, must override the handle
/// method and call its specific handlers inside it.
namespace amadeus {
namespace in {
enum class PacketType : std::uint8_t;
} // namespace in
template <class Session>
class PacketHandler
{
  public:
    /// \brief Result of parser.
    enum class ResultType
    {
        /// \brief EOF. We have completely parsed the packet.
        Good,
        /// \brief Something bad occurred while parsing the packet.
        Bad,
        /// \brief Packet has not been fully parsed yet. We need to read more
        /// data into the input buffer before we can try parsing the packet
        /// again.
        /// \remarks: This only applies to the Packet Header itself.
        Indeterminate
    };

    /// The return type of a handler.
    using HandlerReturnType = std::pair<ResultType, std::size_t>;
    /// The underlying PacketId Type.
    using PacketIdType = std::underlying_type<in::PacketType>::type;
    /// The immutable data from the network buffer.
    using BufferView = asio::const_buffer;

    /// \brief Constructor.
    explicit PacketHandler(Session& _session)
        : session_(_session)
    {
    }

    // \brief Parse some data. The enum return value is 'Good' when a
    /// complete request has been parsed, 'Bad' if the data is invalid,
    /// 'Indeterminate' when more data is required. The std::size_t return
    /// value indicates how much of the input has been consumed. See \ref
    /// HandlerReturnType for more details.
    HandlerReturnType handle(PacketIdType _id, BufferView const _view)
    {
        using namespace in;
        auto const id = static_cast<PacketType>(_id);
        auto const expectedSize = sizeByPacketId(id);

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

        switch (id) {
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

  private:
    /// A reference to the TCPSession.
    Session& session_;

    HandlerReturnType handleHandshakePacket(BufferView const _view) const
    {
        packet_view<in::HandshakePacket> const packet{_view.data()};

        LOG_INFO("handleHandshakePacket called with view: {}.\n", packet);

        boost::uuids::uuid uuid;
        std::memcpy(&uuid, packet->uuid.data(), packet->uuid.size());

        auto const uuidStr = boost::uuids::to_string(uuid);

        /// \brief Returns true if the given UUID is registered in the config,
        /// false otherwise.
        auto const isUUIDRegistered =
            [this](std::string_view _uuid) noexcept -> bool {
            auto const& config = session_.sharedState().config();
            for (auto const& e : config) {
                auto const uuidView = e.template get<std::string_view>();
                if (_uuid == uuidView) {
                    return true;
                }
            }
            return false;
        };

        /// TODO: Handle... respond with HandshakeACKPacket or
        /// HandshakeNAKPacket
        if (isUUIDRegistered(uuidStr.c_str())) {
            // send good handshake
			out::HandshakeACKPacket const packet{};
            session_.writePacket(packet, [this](auto&& bytes_transferred) {
                LOG_INFO("HandshakeACKPacket sent with {} bytes.\n",
                         bytes_transferred);
            });
        } else {
            // send bad handshake
			out::HandshakeNAKPacket const packet{};
            session_.writePacket(packet, [this](auto&& bytes_transferred) {
                LOG_INFO("HandshakeNAKPacket sent with {} bytes.\n",
                         bytes_transferred);
            });
        }

        return std::make_pair(ResultType::Good, packet.size());
    }

    HandlerReturnType handlePongPacket(BufferView const _view) const
    {
        packet_view<in::PongPacket> const packet{_view.data()};

        LOG_INFO("handlePongPacket called with view: {}.\n", packet);

        return std::make_pair(ResultType::Bad, 0);
    }

    HandlerReturnType handleWeatherStatusPacket(BufferView const _view) const
    {
        packet_view<in::WeatherStatusPacket> const packet{_view.data()};

        LOG_INFO("handleWeatherStatusPacket called with view: {}\n", packet);

        return std::make_pair(ResultType::Bad, 0);
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_REQUEST_HANDLER_HH
