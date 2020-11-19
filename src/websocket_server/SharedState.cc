#include "websocket_server/SharedState.hh"
#include "websocket_server/PlainWebSocketSession.hh"
#include "websocket_server/SSLWebSocketSession.hh"

using namespace amadeus;

SharedState::SharedState(std::string _docRoot)
    : docRoot_(std::move(_docRoot))
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

void SharedState::join(PlainWebSocketSession* _session)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    plain_sessions_.emplace(_session);
}

void SharedState::join(SSLWebSocketSession* _session)
{
    std::scoped_lock<std::mutex> lk(mtx_);
    ssl_sessions_.emplace(_session);
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