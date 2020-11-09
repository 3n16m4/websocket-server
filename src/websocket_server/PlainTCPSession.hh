#ifndef WEBSOCKET_SERVER_PLAIN_TCP_SESSION_HH
#define WEBSOCKET_SERVER_PLAIN_TCP_SESSION_HH

#include "websocket_server/TCPSession.hh"

#include <memory>

namespace amadeus {
class PlainTCPSession
    : public TCPSession<PlainTCPSession>
    , public std::enable_shared_from_this<PlainTCPSession>
{
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_TCP_SESSION_HH