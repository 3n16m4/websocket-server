#include "websocket_server/PlainTCPSession.hh"

using namespace amadeus;

void PlainTCPSession::disconnect()
{
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

    if (ec) {
        /// TODO: handle properly
    }
}