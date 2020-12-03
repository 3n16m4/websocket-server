#ifndef WEBSOCKET_SERVER_REQUEST_HANDLER_HH
#define WEBSOCKET_SERVER_REQUEST_HANDLER_HH

#include "websocket_server/asiofwd.hh"

#include <boost/asio/buffer.hpp>

#include <cstdint>
#include <utility>
#include <type_traits>

/// \brief The PacketHandler for incoming µc-TCP requests.
/// A PacketHandler can vary in its design and underlying data structure
/// choice which heavily depends on the network partner (server).
/// Each PacketHandler implementation however, must override the handle
/// method and call its specific handlers inside it.
namespace amadeus {
namespace in {
enum class PacketType : std::uint8_t;
} // namespace in
class SharedState;
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
	explicit PacketHandler(std::shared_ptr<SharedState> const& _state);

    // \brief Parse some data. The enum return value is 'Good' when a
    /// complete request has been parsed, 'Bad' if the data is invalid,
    /// 'Indeterminate' when more data is required. The std::size_t return
    /// value indicates how much of the input has been consumed. See \ref
    /// HandlerReturnType for more details.
    HandlerReturnType handle(PacketIdType _id, BufferView const _view);

  private:
	/// The shared state.
	std::shared_ptr<SharedState> state_;

    HandlerReturnType handleHandshakePacket(BufferView const _view) const;
    HandlerReturnType handlePongPacket(BufferView const _view) const;
    HandlerReturnType handleWeatherStatusPacket(BufferView const _view) const;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_REQUEST_HANDLER_HH
