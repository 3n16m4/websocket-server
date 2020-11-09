#ifndef WEBSOCKET_SERVER_SSL_PROTOCOL_DETECTOR_HH
#define WEBSOCKET_SERVER_SSL_PROTOCOL_DETECTOR_HH

#include "websocket_server/ProtocolDetector.hh"

#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>

#include <memory>

namespace amadeus {
class SSLProtocolDetector
    : public ProtocolDetector<SSLProtocolDetector>
    , public std::enable_shared_from_this<SSLProtocolDetector>
{
  private:
    /// The underlying SSL TCP stream.
    beast::ssl_stream<beast::tcp_stream> stream_;

  public:
    SSLProtocolDetector(beast::tcp_stream&& _stream, ssl::context& _ctx,
                        beast::flat_buffer&& _buffer)
        : ProtocolDetector<SSLProtocolDetector>(std::move(_buffer))
        , stream_(std::move(_stream), _ctx)
    {
    }

    void run()
    {
      /// TODO: perform ssl handshake
      this->run();
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SSL_PROTOCOL_DETECTOR_HH