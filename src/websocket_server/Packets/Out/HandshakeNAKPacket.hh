#ifndef WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH
#define WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH

namespace amadeus {
#pragma pack(push, 1)
/// \brief Defines the HandshakeReason enum which describes the reason for the
/// HandshakeNAKPacket in detail.
enum class HandshakeReason : std::uint8_t
{
    /// StationId was already assigned on the server.
    ReasonStationIdAlready = 1 << 0,
    /// StationId is invalid because it could not be found.
    ReasonStationIdInvalid = 1 << 1,
    /// Wrong UUID.
    ReasonUUIDInvalid = 1 << 2,
    /// Wrong UUID format.
    ReasonUUIDInvalidFormat = 1 << 3,
};
namespace out {
/// \brief Defines the HandshakeNAKPacket which is sent by the server in case
/// the TCP Client was not successfully authenticated.
struct HandshakeNAKPacket
{
    /// Packet header.
    std::uint8_t header{0x02};
    /// The reason for the handshake failure.
    HandshakeReason reason;
};
#pragma pack(pop)
} // namespace out
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_OUT_HANDSHAKE_NAK_PACKET_HH
