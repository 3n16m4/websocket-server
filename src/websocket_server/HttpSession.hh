#ifndef WEBSOCKET_SERVER_HTTP_SESSION_HH
#define WEBSOCKET_SERVER_HTTP_SESSION_HH

#include "websocket_server/asiofwd.hh"
#include "websocket_server/WebSocketSessionFactory.hh"
#include "websocket_server/SharedState.hh"
#include "websocket_server/Common.hh"
#include "websocket_server/Logger.hh"

#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/string.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/beast/version.hpp>

#include <optional>
#include <memory>
#include <string>
#include <stdexcept>

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path);

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(beast::string_view base, beast::string_view path);

namespace amadeus {
// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template <class Body, class Allocator, class Send>
void handle_request(beast::string_view doc_root,
                    http::request<Body, http::basic_fields<Allocator>>&& req,
                    Send&& send)
{
    // Returns a bad request response
    auto const bad_request = [&req](beast::string_view why) {
        http::response<http::string_body> res{http::status::bad_request,
                                              req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found = [&req](beast::string_view target) {
        http::response<http::string_body> res{http::status::not_found,
                                              req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() =
            "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error = [&req](beast::string_view what) {
        http::response<http::string_body> res{
            http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if (req.method() != http::verb::get && req.method() != http::verb::head)
        return send(bad_request("Unknown HTTP-method"));

    // Request path must be absolute and not contain "..".
    if (req.target().empty() || req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos)
        return send(bad_request("Illegal request-target"));

    // Build the path to the requested file
    std::string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/')
        path.append("index.html");

    // Attempt to open the file
    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if (ec == beast::errc::no_such_file_or_directory)
        return send(not_found(req.target()));

    // Handle an unknown error
    if (ec)
        return send(server_error(ec.message()));

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if (req.method() == http::verb::head) {
        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

    // Respond to GET request
    http::response<http::file_body> res{
        std::piecewise_construct, std::make_tuple(std::move(body)),
        std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
}

/// CRTP is used here to avoid code duplication and virtual function calls.
/// This is not only beneficial for performance but also to allow SSL
/// HttpSessions and regular HttpSessions to work with the same code.
/// \brief Provides a simple default HttpSession implementation.
template <class Derived>
class HttpSession
{
  private:
    /// The shared state.
    std::shared_ptr<SharedState> state_;
    /// The HTTP Request parser.
    std::optional<http::request_parser<http::string_body>> parser_;

    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief CompletionToken for the asynchronous read operation.
    void onRead(beast::error_code const& _error, std::size_t _bytesTransferred)
    {
        boost::ignore_unused(_bytesTransferred);

        // Was the connection closed?
        if (_error == http::error::end_of_stream) {
            return derived().disconnect();
        }

        // handle any other error.
        if (_error) {
            /// TODO: handle properly
            LOG_ERROR("Read error: {}\n", _error.message());
            return;
        }

        // was it an http upgrade?
        if (websocket::is_upgrade(parser_->get())) {
            // WebSocketSession has its own timeout, disable it.
            beast::get_lowest_layer(derived().stream()).expires_never();

            // Create the WebSocket session and take ownership of the underlying
            // socket.
            return make_websocket_session(derived().releaseStream())
                ->run(std::move(parser_->release()));
        }

        // handle the request
        handle_request(
            state_->docRoot(), parser_->release(), [this](auto&& response) {
                // The lifetime of the message has to extend
                // for the duration of the async operation so
                // we use a shared_ptr to manage it.
                using response_type =
                    typename std::decay<decltype(response)>::type;
                auto sp = std::make_shared<response_type>(
                    std::forward<decltype(response)>(response));

                // Write the response
                http::async_write(derived().stream(), *sp,
                                  [self = derived().shared_from_this(),
                                   sp](auto&& ec, auto&& bytes_transferred) {
                                      self->onWrite(ec, bytes_transferred,
                                                    sp->need_eof());
                                  });
            });
    }

    /// \brief CompletionToken for the asynchronous write operation.
    void onWrite(beast::error_code const& _error, std::size_t _bytesTransferred,
                 bool _close)
    {
        boost::ignore_unused(_bytesTransferred);

        if (_error) {
            /// TODO: handle..
            return;
        }

        if (_close) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return derived().disconnect();
        }

        // Read another request
        doRead();
    }

  public:
    /// \brief Constructor.
    HttpSession(beast::flat_buffer&& _buffer,
                std::shared_ptr<SharedState> const& _state)
        : buffer_(std::move(_buffer))
        , state_(_state)
    {
    }

    /// \brief Starts the asynchronous read operation.
    void doRead()
    {
        LOG_DEBUG("HttpSession::doRead()\n");

        parser_.emplace();

        // Make sure that we filter out requests that are too large.
        parser_->body_limit(10000);

        beast::get_lowest_layer(derived().stream()).expires_after(Timeout);

        http::async_read(derived().stream(), buffer_, *parser_,
                         [self = derived().shared_from_this()](
                             auto&& ec, auto&& bytes_transferred) {
                             self->onRead(ec, bytes_transferred);
                         });
    }

    /// \brief Starts the asynchronous write operation.
    void doWrite()
    {
        throw std::runtime_error("NotImplemented");
    }

  protected:
    /// The underlying buffer for incoming HTTP Requests.
    beast::flat_buffer buffer_;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_HTTP_SESSION_HH