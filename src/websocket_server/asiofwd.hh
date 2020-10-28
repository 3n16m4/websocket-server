#ifndef WEBSOCKET_SERVER_ASIOFWD_HH
#define WEBSOCKET_SERVER_ASIOFWD_HH

#include <boost/asio/ts/netfwd.hpp>

/// Forward declarations.
namespace boost {
namespace beast {
}
} // namespace boost

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

#endif // !WEBSOCKET_SERVER_ASIOFWD_HH