#ifndef WEBSOCKET_SERVER_DETECT_SESSION_HH
#define WEBSOCKET_SERVER_DETECT_SESSION_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/Common.hh"

#include <boost/asio/dispatch.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/detect_ssl.hpp>

/// \brief Helper class to detect SSL handshakes.
/// This code works for both HTTP and Raw TCP Sessions.
namespace amadeus {
/// Forward declarations
class SharedState;
template <class Derived, class BufferType = beast::flat_buffer>
class DetectSession
{
  protected:
    /// A reference to the main SSL-Context.
    ssl::context& ctx_;
    /// The shared state.
    std::shared_ptr<SharedState> state_;
    /// The main TCP Stream.
    beast::tcp_stream stream_;
    /// The underlying buffer for determining an SSL request.
    BufferType buffer_;

  private:
    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief Detect a TLS/SSL handshake asynchronously on a stream.
    void onRun()
    {
        stream_.expires_after(std::chrono::seconds(Timeout));

        beast::async_detect_ssl(
            stream_, buffer_,
            [self = derived().shared_from_this()](
                beast::error_code const& error, bool result) {
                if (error) {
                  /// TODO: handle...
                }
                self->onDetect(result);
            });
    }

  public:
    /// \brief Constructor.
    DetectSession(tcp::socket&& _socket, ssl::context& _ctx,
                  std::shared_ptr<SharedState> const& _state)
        : stream_(std::move(_socket))
        , ctx_(_ctx)
        , state_(_state)
    {
    }

    /// \brief Starts the SSL detector.
    void doRead()
    {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        asio::dispatch(
            stream_.get_executor(),
            [self = derived().shared_from_this()]() { self->onRun(); });
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_DETECT_SESSION_HH