#ifndef WEBSOCKET_SERVER_SHARED_STATE_HH
#define WEBSOCKET_SERVER_SHARED_STATE_HH

#include "websocket_server/CommandLineInterface.hh"

#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>

#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>

// TODO: Delete PlainTCPSessions because they are not needed and should not be
// allowed in the webserver.

namespace amadeus {
/// \brief This class is responsible for keeping track of all connected
/// websocket sessions. It provides functions for joining, leaving sessions and
/// sending messages to all websocket sessions. In essence, it simply holds
/// server data related to the websocket sessions and is thus shared by every
/// websocket session.

enum class StationId : std::uint8_t;

struct WeatherStatusNotification
{
    StationId id;
    float temperature;
    float humidity;
};

template <typename SessionType>
struct WebSocketSessionCtx
{
    // static_assert(std::is_base_of_v<SessionType, WebSocketSession>());
    /// Can either be PlainWebSocketSession or SSLWebSocketSession.
    SessionType* session;
    /// Called each time a new weather response was received from the TCP
    /// Session.
    std::function<void(WeatherStatusNotification)> callback;
};

class PlainWebSocketSession;
class SSLWebSocketSession;
class PlainTCPSession;
class SSLTCPSession;
class SharedState
{
  private:
    /// The document root.
    std::string const docRoot_;
    /// The JSON config.
    JSON const& config_;
    /// Protects the sessions list.
    std::mutex mtx_;
    /// A set to track all plain websockets.
    std::unordered_map<boost::uuids::uuid,
                       WebSocketSessionCtx<PlainWebSocketSession>,
                       boost::hash<boost::uuids::uuid>>
        plain_sessions_;
    /// A set to track all ssl websockets.
    std::unordered_map<boost::uuids::uuid,
                       WebSocketSessionCtx<SSLWebSocketSession>,
                       boost::hash<boost::uuids::uuid>>
        ssl_sessions_;
    /// A hashmap for all plain tcp sessions bound to a unique stationId.
    std::unordered_map<StationId, PlainTCPSession*> plain_tcp_sessions_;
    /// A hashmap for all ssl tcp sessions bound to a unique stationId.
    std::unordered_map<StationId, SSLTCPSession*> ssl_tcp_sessions_;

    /// \brief Returns a weak_ptr for a PlainWebSocketSession.
    std::weak_ptr<PlainWebSocketSession>
    weak_from_this(PlainWebSocketSession* _session) noexcept;

    /// \brief Returns a weak_ptr for an SSLWebSocketSession.
    std::weak_ptr<SSLWebSocketSession>
    weak_from_this(SSLWebSocketSession* _session) noexcept;

    /// \brief Returns a weak_ptr for a PlainTCPSession.
    std::weak_ptr<PlainTCPSession>
    weak_from_this(PlainTCPSession* _session) noexcept;

    /// \brief Returns a weak_ptr for an SSLTCPSession.
    std::weak_ptr<SSLTCPSession>
    weak_from_this(SSLTCPSession* _session) noexcept;

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
            for (auto [_, ctx] : plain_sessions_) {
                _sequence.emplace_back(weak_from_this(ctx.session));
            }
        } else if constexpr (std::is_same_v<SessionType, SSLWebSocketSession>) {
            _sequence.reserve(ssl_sessions_.size());
            for (auto [_, ctx] : ssl_sessions_) {
                _sequence.emplace_back(weak_from_this(ctx.session));
            }
        }
    }

    /// \remarks Not Thread-Safe. Must be called with a held lock.
    std::function<void(WeatherStatusNotification)>
    findPlainWebSocketSession(boost::uuids::uuid const& _uuid)
    {
        if (auto const it = plain_sessions_.find(_uuid);
            it != std::end(plain_sessions_)) {
            auto wp = weak_from_this(it->second.session);
            if (auto sp = wp.lock()) {
                return it->second.callback;
            }
            // sp has expired
            return nullptr;
        }
        // not found
        return nullptr;
    }

    /// TODO: same thing for SSLWebSocketSession

  public:
    /// \brief Constructor.
    /// \param _docRoot The document resources directory.
    SharedState(std::string _docRoot, JSON const& _config);

    ~SharedState();

    /// \brief Returns the document root.
    std::string const& docRoot() const noexcept;

    /// \brief Returns the JSON config.
    JSON const& config() const noexcept;

    /// \brief Join a PlainWebSocketSession and insert it into the list.
    /// \param _session The PlainWebSocketSession pointer.
    /// \remarks Thread-Safe.
    bool join(boost::uuids::uuid _uuid,
              WebSocketSessionCtx<PlainWebSocketSession> _ctx);

    /// \brief Join an SSLWebSocketSession and insert it into the list.
    /// \param _session The SSLWebSocketSession pointer.
    /// \remarks Thread-Safe.
    bool join(boost::uuids::uuid _uuid,
              WebSocketSessionCtx<SSLWebSocketSession> _ctx);

    /// \brief Join a PlainTCPSession and insert it into the list.
    /// \param _session The PlainTCPSession pointer.
    /// \param _id The StationId.
    /// \remarks Thread-Safe.
    bool join(StationId _id, PlainTCPSession* _session);

    /// \brief Join a SSLTCPSession and insert it into the list.
    /// \param _session The SSLTCPSession pointer.
    /// \param _id The StationId.
    /// \remarks Thread-Safe.
    bool join(StationId _id, SSLTCPSession* _session);

    /// \remarks Thread-Safe.
    template <typename SessionType>
    void leave(boost::uuids::uuid const& _uuid)
    {
        std::scoped_lock<std::mutex> lk(mtx_);

        if constexpr (std::is_same_v<SessionType, PlainWebSocketSession>) {
            plain_sessions_.erase(_uuid);
        } else if constexpr (std::is_same_v<SessionType, SSLWebSocketSession>) {
            ssl_sessions_.erase(_uuid);
        }
    }

    /// \remarks Thread-Safe.
    template <typename SessionType>
    void leave(StationId _id)
    {
        std::scoped_lock<std::mutex> lk(mtx_);

        if constexpr (std::is_same_v<SessionType, PlainTCPSession>) {
            plain_tcp_sessions_.erase(_id);
        } else if constexpr (std::is_same_v<SessionType, SSLTCPSession>) {
            ssl_tcp_sessions_.erase(_id);
        }
    }

    template <typename SessionType>
    std::function<void(WeatherStatusNotification)>
    findWebSocketSession(boost::uuids::uuid const& _uuid)
    {
        std::scoped_lock<std::mutex> lk(mtx_);
        if constexpr (std::is_same_v<SessionType, PlainWebSocketSession>) {
            return findPlainWebSocketSession(_uuid);
        } else if constexpr (std::is_same_v<SessionType, SSLWebSocketSession>) {
            /// TODO: same thing
        }
        return nullptr;
    }

    // TODO: Only plain_tcp_sessions_ for now. Replace plain_tcp_sessions_ with
    // ssl_tcp_sessions_ upon release.
    std::shared_ptr<PlainTCPSession> findStation(StationId _key)
    {
        std::scoped_lock<std::mutex> lk(mtx_);
        if (auto const it = plain_tcp_sessions_.find(_key);
            it != std::end(plain_tcp_sessions_)) {
            auto wp = weak_from_this(it->second);
            if (auto sp = wp.lock()) {
                return sp;
            }
            // sp has expired
            return nullptr;
        }
        // not found
        return nullptr;
    }

    std::vector<StationId> allStationIds()
    {
        std::vector<StationId> ids;

        std::scoped_lock<std::mutex> lk(mtx_);
        {
            ids.reserve(plain_tcp_sessions_.size() + ssl_tcp_sessions_.size());

            for (auto const& [stationId, _] : plain_tcp_sessions_) {
                ids.emplace_back(stationId);
            }
            for (auto const& [stationId, _] : ssl_tcp_sessions_) {
                ids.emplace_back(stationId);
            }
        }

        return ids;
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
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SHARED_STATE_HH
