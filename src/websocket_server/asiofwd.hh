#ifndef WEBSOCKET_SERVER_ASIOFWD_HH
#define WEBSOCKET_SERVER_ASIOFWD_HH

#include <boost/asio/ts/netfwd.hpp>

/// Forward declarations
namespace boost {
namespace beast {
namespace http {
}
namespace websocket {
}
} // namespace beast
} // namespace boost

/// Boost/Asio
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

// Boost/Beast
namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;

#endif // !WEBSOCKET_SERVER_ASIOFWD_HH