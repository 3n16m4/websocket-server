#ifndef WEBSOCKET_SERVER_SHARED_STATE_HH
#define WEBSOCKET_SERVER_SHARED_STATE_HH

#include "websocket_server/PlainWebSocketSession.hh"
#include "websocket_server/SSLWebSocketSession.hh"

#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace amadeus {
/// \brief This class is responsible for keeping track of all connected
/// websocket sessions. It provides functions for joining, leaving sessions and
/// sending messages to all websocket sessions. In essence, it simply holds
/// server data related to the websocket sessions and is thus shared by every
/// websocket session.
class SharedState
{
  private:
    /// The document root.
    std::string const docRoot_;
    /// Protects the sessions list.
    std::mutex mtx_;
    /// A set to track all plain websockets.
    std::unordered_set<PlainWebSocketSession*> plain_sessions_;
    /// A set to track all ssl websockets.
    std::unordered_set<SSLWebSocketSession*> ssl_sessions_;

  public:
    /// \brief Constructor.
    /// \param _docRoot The document resources directory.
    SharedState(std::string _docRoot);

    /// \brief Returns the document root.
    std::string const& docRoot() const noexcept;

    /// \brief Join a websocket session and insert it into the list.
    /// \tparam SessionType The SessionType can either be PlainWebSocketSession
    /// or SSLWebSocketSession.
    /// \remarks Thread-Safe.
    template <typename SessionType>
    void join(SessionType* _session)
    {
        if constexpr (std::is_same_v<SessionType, PlainWebSocketSession>) {
            plain_sessions_.emplace(_session);
        } else if constexpr (std::is_same_v<SessionType, SSLWebSocketSession>) {
            ssl_sessions_.emplace(_session);
        }
    }

    /// \brief Leave a websocket session and erase it from the list.
    /// \tparam SessionType The SessionType can either be PlainWebSocketSession
    /// or SSLWebSocketSession.
    /// \remarks Thread-Safe.
    template <typename SessionType>
    void leave(SessionType* _session)
    {
        if constexpr (std::is_same_v<SessionType, PlainWebSocketSession>) {
            plain_sessions_.erase(_session);
        } else if constexpr (std::is_same_v<SessionType, SSLWebSocketSession>) {
            ssl_sessions_.erase(_session);
        }
    }

    /// \brief Broadcast a message to all connected websocket sessions from the
    /// list.
    /// \tparam SessionType The SessionType can either be PlainWebSocketSession
    /// or SSLWebSocketSession.
    /// \remarks Thread-Safe.
    template <typename SessionType>
    void send(std::string&& _message)
    {
        // We store the message in a shared_ptr to make sure that the message is
        // persistent.
        auto const s = std::make_shared<std::string const>(std::move(_message));

        std::vector<std::weak_ptr<SessionType>> v;
        {
            std::scoped_lock<std::mutex> lk(mtx_);
            copySessions<SessionType>(v);
        }

        // Try to obtain a shared_ptr from each weak_ptr and send the message to
        // the session.
        for (auto const& wp : v) {
            if (auto sp = wp.lock()) {
                sp->send(s);
            }
        }
    }

    /// \brief For performance reasons we'll copy all weak_ptr from the session
    /// list into a local vector. This is done to avoid holding a lock while
    /// broadcasting the message.
    /// \tparam SessionType The SessionType can either be PlainWebSocketSession
    /// or SSLWebSocketSession.
    /// \tparam ContainerSequence The container in which to copy the sessions
    /// to. The container must be of type std::vector<SessionType*>.
    template <typename SessionType, typename ContainerSequence>
    inline void copySessions(ContainerSequence&& _sequence)
    {
        static_assert(
            std::is_same_v<ContainerSequence, std::vector<SessionType*>>,
            "The ContainerSequence must be of type std::vector<SessionType*>!");

        if constexpr (std::is_same_v<SessionType, PlainWebSocketSession>) {
            _sequence.reserve(plain_sessions_.size());
            for (auto* session : plain_sessions_) {
                _sequence.emplace_back(session->weak_from_this());
            }
        } else if constexpr (std::is_same_v<SessionType, SSLWebSocketSession>) {
            _sequence.reserve(ssl_sessions_.size());
            for (auto* session : ssl_sessions_) {
                _sequence.emplace_back(session->weak_from_this());
            }
        }
    }

    /// \brief Returns the size by the given SessionType.
    /// \tparam SessionType The SessionType can either be PlainWebSocketSession
    /// or SSLWebSocketSession.
    template <typename SessionType>
    auto size() const noexcept
    {
        if constexpr (std::is_same_v<SessionType, PlainWebSocketSession>) {
            return plain_sessions_.size();
        } else if constexpr (std::is_same_v<SessionType, SSLWebSocketSession>) {
            return ssl_sessions_.size();
        }
    }
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SHARED_STATE_HH
