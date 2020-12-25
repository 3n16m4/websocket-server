#include "websocket_server/SharedState.hh"
#include "websocket_server/PlainWebSocketSession.hh"
#include "websocket_server/SSLWebSocketSession.hh"
#include "websocket_server/PlainTCPSession.hh"
#include "websocket_server/SSLTCPSession.hh"

using namespace amadeus;

SharedState::SharedState(std::string _docRoot, JSON const& _config)
    : docRoot_(std::move(_docRoot))
    , config_(_config)
{
    LOG_DEBUG("SharedState::SharedState()\n");
}

SharedState::~SharedState()
{
    LOG_DEBUG("SharedState::~SharedState()\n");
}

std::weak_ptr<SSLWebSocketSession>
SharedState::weak_from_this(SSLWebSocketSession* _session) noexcept
{
    return _session->weak_from_this();
}

std::weak_ptr<PlainWebSocketSession>
SharedState::weak_from_this(PlainWebSocketSession* _session) noexcept
{
    return _session->weak_from_this();
}

std::weak_ptr<PlainTCPSession>
SharedState::weak_from_this(PlainTCPSession* _session) noexcept
{
    return _session->weak_from_this();
}

/// \brief Returns a weak_ptr for an SSLTCPSession.
std::weak_ptr<SSLTCPSession>
SharedState::weak_from_this(SSLTCPSession* _session) noexcept
{
    return _session->weak_from_this();
}

std::string const& SharedState::docRoot() const noexcept
{
    return docRoot_;
}

JSON const& SharedState::config() const noexcept
{
    return config_;
}

bool SharedState::join(boost::uuids::uuid _uuid,
                       WebSocketSessionCtx<PlainWebSocketSession> _ctx)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    auto const [_, joined] =
        plain_sessions_.try_emplace(std::move(_uuid), std::move(_ctx));
    return joined;
}

bool SharedState::join(boost::uuids::uuid _uuid,
                       WebSocketSessionCtx<SSLWebSocketSession> _ctx)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    auto const [_, joined] =
        ssl_sessions_.try_emplace(std::move(_uuid), std::move(_ctx));
    return joined;
}

bool SharedState::join(StationId _id, PlainTCPSession* _session)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    auto const [_, joined] = plain_tcp_sessions_.try_emplace(_id, _session);
    return joined;
}

bool SharedState::join(StationId _id, SSLTCPSession* _session)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    auto const [_, joined] = ssl_tcp_sessions_.try_emplace(_id, _session);
    return joined;
}

