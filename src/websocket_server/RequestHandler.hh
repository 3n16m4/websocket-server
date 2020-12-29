#ifndef WEBSOCKET_SERVER_REQUEST_HANDLER_HH
#define WEBSOCKET_SERVER_REQUEST_HANDLER_HH

#include "websocket_server/asiofwd.hh"

#include <boost/asio/buffer.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

namespace amadeus {
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
    Indeterminate,
    /// \brief The requested payload is too big for the server to handle. The
    /// caller should simply send back a bad request instead of gracefully
    /// closing the remote peer.
    PayloadTooBig,
};

/// The return type of a handler.
using HandlerReturnType = std::pair<ResultType, std::size_t>;
/// The immutable data from the network buffer.
using BufferView = asio::const_buffer;
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_REQUEST_HANDLER_HH
