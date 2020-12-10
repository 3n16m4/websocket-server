#ifndef WEBSOCKET_SERVER_WEBSOCKET_SESSION_HH
#define WEBSOCKET_SERVER_WEBSOCKET_SESSION_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/SharedState.hh"

#include <boost/beast/http/message.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/websocket/stream_base.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/websocket/option.hpp>
#include <boost/beast/websocket/error.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/post.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace amadeus {
/// CRTP is used here to avoid code duplication and virtual function calls.
/// This is not only beneficial for performance but also to allow SSL websocket
/// sessions and regular websocket sessions to work with the same code.
/// \brief Provides a simple default WebSocketSession implementation.
class SharedState;
template <class Derived>
class WebSocketSession
{
  protected:
    /// The shared state.
    std::shared_ptr<SharedState> state_;
    /// The underlying buffer for requests.
    beast::flat_buffer buffer_;

    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief Accepts the WebSocket handshake.
    template <class Body, class Allocator>
    void onRun(http::request<Body, http::basic_fields<Allocator>> _req)
    {
        auto& ws = derived().stream();

        // Set the timeout for the websocket and enable ping packets to be sent.
        websocket::stream_base::timeout const timeout{
            std::chrono::seconds(HandshakeTimeout), std::chrono::seconds(10s),
            true};
        ws.set_option(timeout);

        // Set a decorator to change the user-agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res) {
                res.set(http::field::server, SERVER_VERSION_STRING);
            }));

        // Accept the websocket handshake
        ws.async_accept(_req, [self = derived().shared_from_this()](auto&& ec) {
            self->onAccept(ec);
        });
    }

    /// \brief CompletionToken for the asynchronous accept operation.
    void onAccept(beast::error_code const& _error)
    {
        if (_error) {
            LOG_ERROR("Accept error: {}\n", _error.message());
        } else {
            // Add this session to the list of active sessions
            state_->join(&derived());

            // Register control frame callback for pong.
            auto& ws = derived().stream();
            ws.control_callback([this](auto&& _kind, auto&& _payload) {
                onControlMessage(_kind, _payload);
            });

            // Read a message
            doRead();
        }
    }

    void onControlMessage(websocket::frame_type _kind,
                          beast::string_view _payload)
    {
        LOG_DEBUG("Received control frame: {} {}\n", static_cast<int>(_kind),
                  _payload);
    }

    /// \brief Asynchronously reads a message into the buffer.
    void doRead()
    {
        auto& ws = derived().stream();
        ws.async_read(buffer_, [self = derived().shared_from_this()](
                                   auto&& ec, auto&& bytes_transferred) {
            self->onRead(ec, bytes_transferred);
        });
    }

    /// \brief CompletionToken for the asynchronous read operation.
    void onRead(beast::error_code const& _error, std::size_t _bytesTransferred)
    {
        // The WebSocket stream was gracefully closed at both endpoints
        if (_error == websocket::error::closed) {
            LOG_DEBUG("WebSocketSession was gracefully closed.\n");
            return;
        }

        if (_error) {
            LOG_ERROR("Read error: {}\n", _error.message());
            return;
        }

        /// TODO: handle the request...
        /// ...

        // Just echo the message for now
        auto& ws = derived().stream();

        // Set the message type to text
        ws.text(ws.got_text());
        ws.async_write(buffer_.data(),
                       [self = derived().shared_from_this()](
                           auto&& ec, auto&& bytes_transferred) {
                           self->onWrite(ec, bytes_transferred);
                       });
    }

    /// \brief CompletionToken for the asynchronous write operation.
    void onWrite(beast::error_code const& _error, std::size_t _bytesTransferred)
    {
        if (_error) {
            LOG_ERROR("Write error: {}\n", _error.message());
            return;
        }

        /// TODO: Erase the message from the front of the queue
        /// send the next message from the queue if there's any.

        // Broadcast the message to all connections.
        // state_->send<Derived>(beast::buffers_to_string(buffer_.data()));

        // Clear the buffer
        buffer_.consume(buffer_.size());

        // Read another message
        doRead();
    }

    /// \brief CompletionToken for the send operation.
    void onSend(std::shared_ptr<std::string const> const& _message)
    {
        /// TODO: queue work..
        /// Send the message from the front of the queue.
    }

  public:
    /// \brief Creates a WebSocket Session.
    explicit WebSocketSession(std::shared_ptr<SharedState> const& _state)
        : state_(_state)
    {
        LOG_DEBUG("WebSocketSession::WebSocketSession()\n");
    }

    /// \brief Leaves the WebSocket Session.
    ~WebSocketSession()
    {
        LOG_DEBUG("WebSocketSession::~WebSocketSession()\n");
        state_->leave(&derived());
    }

    /// \brief Start the asynchronous operation.
    template <class Body, class Allocator>
    void run(http::request<Body, http::basic_fields<Allocator>> _req)
    {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        asio::dispatch(
            derived().stream().get_executor(),
            [self = derived().shared_from_this(), _req = std::move(_req)] {
                self->onRun(std::move(_req));
            });
    }

    /// \brief Sends a given message to all connected websocket sessions.
    /// Called from the SharedState for each WebSocketSession.
    void send(std::shared_ptr<std::string const> const& _message)
    {
        // Post our work to the strand, this ensures
        // that the members of `this` will not be
        // accessed concurrently.
        asio::post(derived().stream().get_executor(),
                   [self = derived().shared_from_this(), _message] {
                       self->onSend(_message);
                   });
    }

  protected:
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_WEBSOCKET_SESSION_HH
