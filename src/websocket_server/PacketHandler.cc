#include "websocket_server/PacketHandler.hh"
#include "websocket_server/Packets/In.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/utils/packet_view.hh"

using namespace amadeus;

PacketHandler::PacketHandler(std::shared_ptr<SharedState> const& _state)
    : state_(_state)
{
}

PacketHandler::HandlerReturnType PacketHandler::handle(PacketIdType _id,
                                                       BufferView const _view)
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
        LOG_ERROR("Unable to find handler callback for PacketId '{0:#04x}'.\n",
                  _id);
    }

    return std::make_pair(ResultType::Bad, 0);
}

PacketHandler::HandlerReturnType
PacketHandler::handleHandshakePacket(BufferView const _view) const
{
    packet_view<HandshakePacket> const packet{_view.data()};

    LOG_INFO("handleHandshakePacket called with view: {}.\n", packet);

    /// TODO: Handle... respond with HandshakeACKPacket or HandshakeNAKPacket

    return std::make_pair(ResultType::Good, packet.size());
}

PacketHandler::HandlerReturnType
PacketHandler::handlePongPacket(BufferView const _view) const
{
    return std::make_pair(ResultType::Bad, 0);
}

PacketHandler::HandlerReturnType
PacketHandler::handleWeatherStatusPacket(BufferView const _view) const
{
    return std::make_pair(ResultType::Bad, 0);
}
