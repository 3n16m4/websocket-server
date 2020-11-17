#include "websocket_server/SharedState.hh"

using namespace amadeus;

SharedState::SharedState(std::string _docRoot)
    : docRoot_(std::move(_docRoot))
{
}

std::string const& SharedState::docRoot() const noexcept
{
    return docRoot_;
}