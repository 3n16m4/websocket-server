#ifndef WEBSOCKET_SERVER_TCP_SESSION_HH
#define WEBSOCKET_SERVER_TCP_SESSION_HH

#include "websocket_server/asiofwd.hh"

#include <boost/beast/core/flat_buffer.hpp>

namespace amadeus {
template <class Derived>
class TCPSession
{
  private:
    /// \brief Helper function to access the derived class.
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    /// \brief Starts the asynchronous read operation.
    void doRead();

  public:

  protected:
    /// The underlying buffer for incoming requests.
    beast::flat_buffer buffer_;
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_TCP_SESSION_HH