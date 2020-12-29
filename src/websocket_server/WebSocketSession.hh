#ifndef WEBSOCKET_SERVER_WEBSOCKET_SESSION_HH
#define WEBSOCKET_SERVER_WEBSOCKET_SESSION_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/Logger.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/SharedState.hh"
#include "websocket_server/WebSocketRequestHandler.hh"

#include <boost/beast/http/message.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/websocket/stream_base.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/websocket/option.hpp>
#include <boost/beast/websocket/error.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/post.hpp>
#include <boost/uuid/random_generator.hpp>

#include <magic_enum.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <deque>
/// TODO: remove me
#include <iostream>

namespace amadeus {
/// CRTP is used here to avoid code duplication and virtual function calls.
/// This is not only beneficial for performance but also to allow SSL websocket
/// sessions and regular websocket sessions to work with the same code.
/// \brief Provides a simple default WebSocketSession implementation.
class SharedState;
template <class Derived>
class WebSocketSession
{
  private:
    /// The shared state.
    std::shared_ptr<SharedState> state_;
    /// The underlying buffer for requests.
    beast::flat_buffer buffer_;
    /// The underlying WebSocketRequestHandler for the WebSocketSession.
    // WebSocketRequestHandler<WebSocketSession> handler_;
    WebSocketRequestHandler<Derived> handler_;
    /// Each session is uniquely identified with a random UUID.
    boost::uuids::uuid uuid_;
    /// Each session holds a number of requested weather stations.
    int numRequestedStations_{0};
    /// Queue for the Weather statuses.
    std::deque<WeatherStatusNotification> notifications_;

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
            // Generate a UUID to uniquely associate this websocket sesssion.
            uuid_ = boost::uuids::random_generator()();

            // Add this session to the list of active sessions
            WebSocketSessionCtx<Derived> ctx;
            ctx.session = &derived();
            ctx.callback = [self =
                                derived().shared_from_this()](auto const& arg) {
                return self->onWeatherStatusNotification(arg);
            };

            state_->join(std::move(uuid_), std::move(ctx));

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

        auto constexpr PayloadFieldLength = 2U;
        if (_bytesTransferred < PayloadFieldLength) {
            LOG_ERROR("Incoming request must be at least 2 bytes long!\n");
            return;
        }

        if (buffer_.size() < _bytesTransferred) {
            LOG_ERROR("Payload is too big!\n");
            return;
        }

        LOG_DEBUG("Got {} bytes\n", _bytesTransferred);

        while (buffer_.size()) {
            auto const [status, bytesParsed] = handler_.handle(buffer_.cdata());

            LOG_DEBUG("Status: {}, Bytes parsed: {}\n",
                      magic_enum::enum_name<ResultType>(status), bytesParsed);

            // Pop off the current packet we read.
            buffer_.consume(bytesParsed);

            switch (status) {
            case ResultType::Good: {
                // do we still need to parse?
                if (buffer_.size()) {
                    continue;
                }
                // we are done, read another request.
                return doRead();
            } break;
            case ResultType::Bad: {
                auto& stream = derived().stream();
                return stream.async_close(
                    beast::websocket::close_code::none,
                    [self = derived().shared_from_this()](auto&& ec) {
                        if (ec) {
                            LOG_ERROR("Error on closing WebSocketSession: {}\n",
                                      ec.message());
                        } else {
                            LOG_DEBUG("WebSocket sent close frame to peer.\n");
                        }
                    });
            } break;
            case ResultType::Indeterminate: {
                // TODO: Read more data into buffer.
            } break;
            case ResultType::PayloadTooBig: {
                // TODO: Send a bad request.
            } break;
            }
        }
    }

  public:
    /// \brief Creates a WebSocket Session.
    explicit WebSocketSession(std::shared_ptr<SharedState> _state)
        : state_(std::move(_state))
        //, handler_(*this)
        , handler_(derived())
    {
        LOG_DEBUG("WebSocketSession::WebSocketSession()\n");
    }

    /// \brief Leaves the WebSocket Session.
    ~WebSocketSession()
    {
        LOG_DEBUG("WebSocketSession::~WebSocketSession()\n");
        state_->leave<Derived>(uuid_);
    }

    /// \brief Returns the underlying SharedState reference.
    SharedState const& sharedState() const noexcept
    {
        return *state_.get();
    };

    /// \brief Returns the underlying SharedState reference.
    SharedState& sharedState() noexcept
    {
        return *state_.get();
    };

    /// \brief Returns the UUID.
    boost::uuids::uuid& uuid() noexcept
    {
        return uuid_;
    };

    void numRequestedStations(int _val) noexcept
    {
        numRequestedStations_ = _val;
    }

    template <typename CompletionHandler>
    void writeRequest(JSON _request, CompletionHandler&& _handler)
    {
        // make sure the data is kept alive until it is fully sent
        auto payload = std::move(_request);
        auto const sp = std::make_shared<std::string>(payload.dump());

        auto& ws = derived().stream();
        ws.async_write(
            asio::buffer(*sp), [self = derived().shared_from_this(),
                                _handler = std::move(_handler)](
                                   auto&& error, auto&& bytes_transferred) {
                if (error) {
                    LOG_ERROR("WS write error: {}\n", error.message());
                    return;
                }
                _handler(bytes_transferred);
            });
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

    /// \brief Called each time a new Weather Status Response is received from a
    /// µc.
    void
    onWeatherStatusNotification(WeatherStatusNotification const& _notification)
    {
        LOG_DEBUG("WeatherStatusNotification: {} {}\n",
                  _notification.temperature, _notification.humidity);

        if (notifications_.size() == numRequestedStations_) {
            return;
        }

        // buffer the notifications
        notifications_.emplace_front(_notification);

        // compare the amount of requested weather stations inside
        // callback so that it can send the response back to the web socket
        // session, once we received weather statuses from all stations.
        if (notifications_.size() == numRequestedStations_) {
            // Prepare JSON response for frontend.
            auto response = JSON::object();
            response["id"] = ResponseType::WeatherStatus;

            auto stations = JSON::array();
            while (!notifications_.empty()) {
                auto const element = std::move(notifications_.front());
                notifications_.pop_front();

                std::cout << "element: "
                          << magic_enum::enum_name<StationId>(element.id) << " "
                          << element.temperature << " " << element.humidity
                          << std::endl;

                auto station = JSON::object();
                station["stationId"] = element.id;
                station["temperature"] = element.temperature;
                station["humidity"] = element.humidity;
                /// TODO: obtain time from µc!!! not from server!
                station["time"] = "2021-01-01 14:03:55";

                stations.push_back(std::move(station));
            }
            response["stations"] = std::move(stations);

            // Send response back to frontend.
            writeRequest(std::move(response), [](auto&& bytes_transferred) {
                LOG_DEBUG("WeatherStatus Reponse was sent to frontend with {} "
                          "bytes.\n",
                          bytes_transferred);
            });

            numRequestedStations_ = 0;
        }
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_WEBSOCKET_SESSION_HH
