#include "websocket_server/SharedState.hh"
#include "websocket_server/PlainWebSocketSession.hh"
#include "websocket_server/SSLWebSocketSession.hh"
#include "websocket_server/Logger.hh"

using namespace amadeus;

SharedState::SharedState(std::string _docRoot, JSON const& _config)
    : docRoot_(std::move(_docRoot))
    , config_(_config)
{
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

std::string const& SharedState::docRoot() const noexcept
{
    return docRoot_;
}

JSON const& SharedState::config() const noexcept
{
    return config_;
}

bool SharedState::join(PlainWebSocketSession* _session)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    auto const [_, joined] = plain_sessions_.emplace(_session);
	return joined;
}

bool SharedState::join(SSLWebSocketSession* _session)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    auto const [_, joined] =ssl_sessions_.emplace(_session);
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

void SharedState::leave(PlainWebSocketSession* _session)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    plain_sessions_.erase(_session);
}

void SharedState::leave(SSLWebSocketSession* _session)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    ssl_sessions_.erase(_session);
}

