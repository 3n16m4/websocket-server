#include "websocket_server/PlainHttpSession.hh"

using namespace amadeus;

void PlainHttpSession::disconnect()
{
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

    if (ec) {
        /// TODO: handle properly
    }
}