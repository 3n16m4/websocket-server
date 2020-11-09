#ifndef WEBSOCKET_SERVER_PLAIN_PROTOCOL_DETECTOR_HH
#define WEBSOCKET_SERVER_PLAIN_PROTOCOL_DETECTOR_HH

#include "websocket_server/ProtocolDetector.hh"

#include <boost/beast/core/tcp_stream.hpp>

#include <memory>

namespace amadeus {
class PlainProtocolDetector
    : public ProtocolDetector<PlainProtocolDetector>
    , public std::enable_shared_from_this<PlainProtocolDetector>
{
  private:
    /// The underlying TCP stream.
    beast::tcp_stream stream_;

  public:
    PlainProtocolDetector(beast::tcp_stream&& _stream,
                          beast::flat_buffer&& _buffer)
        : ProtocolDetector<PlainProtocolDetector>(std::move(_buffer))
        , stream_(std::move(_stream))
    {
    }

    void run()
    {
        this->run();
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PLAIN_PROTOCOL_DETECTOR_HH