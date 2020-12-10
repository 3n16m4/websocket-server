#include "websocket_server/PlainHttpSession.hh"
#include "websocket_server/Logger.hh"

using namespace amadeus;

/// \brief Create a plain HTTP Session.
PlainHttpSession::PlainHttpSession(tcp::socket&& _socket,
                                   std::shared_ptr<SharedState> const& _state)
    : HttpSession<PlainHttpSession>(_state)
    , stream_(std::move(_socket))
{
    LOG_DEBUG("PlainHttpSession::PlainHttpSession()\n");
}

PlainHttpSession::~PlainHttpSession()
{
    LOG_DEBUG("PlainHttpSession::~PlainHttpSession()\n");
}

beast::tcp_stream& PlainHttpSession::stream() noexcept
{
    return stream_;
}

beast::tcp_stream PlainHttpSession::releaseStream() noexcept
{
    return std::move(stream_);
}

void PlainHttpSession::run()
{
    this->doRead();
}

void PlainHttpSession::disconnect()
{
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

    if (ec) {
        LOG_ERROR("shutdown error: {}\n", ec.message());
    }
}
