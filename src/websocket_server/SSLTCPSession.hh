#ifndef WEBSOCKET_SERVER_SSL_TCP_SESSION_HH
#define WEBSOCKET_SERVER_SSL_TCP_SESSION_HH

#include "websocket_server/TCPSession.hh"

#include <memory>

namespace amadeus {
class SSLTCPSession
    : public TCPSession<SSLTCPSession>
    , public std::enable_shared_from_this<SSLTCPSession>
{
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_TCP_SESSION_HH