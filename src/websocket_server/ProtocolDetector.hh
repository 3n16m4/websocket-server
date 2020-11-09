#ifndef WEBSOCKET_SERVER_PROTOCOL_DETECTOR_HH
#define WEBSOCKET_SERVER_PROTOCOL_DETECTOR_HH

#include "websocket_server/asiofwd.hh"

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/string_body.hpp>

namespace amadeus {
template <class Derived>
class ProtocolDetector
{
  private:
    /// The HTTP Request parser.
    http::request_parser<http::string_body> parser_;

    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief Performs the asynchronous read operation.
    void doRead()
    {
        /// calls http::async_read with the derived stream (plain or ssl)
        /// read http packet
        /// -  is good?
        ///  - launch SSLHttpSession
        /// -  is bad?
        ///  - launch SSLTCPSession
    }

    /// \brief CompletionToken for the asynchronous read operation.
    void onRead(beast::error_code const& _error, std::size_t _bytesTransferred);

  public:
    /// \brief Constructor.
    ProtocolDetector(beast::flat_buffer&& _buffer)
        : buffer_(std::move(_buffer))
    {
    }

    /// \brief Starts the protocol detection operation.
    void run()
    {
        doRead();
    }

  protected:
    /// The underlying buffer for incoming Requests.
    beast::flat_buffer buffer_;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_PROTOCOL_DETECTOR_HH