#ifndef WEBSOCKET_SERVER_SHARED_STATE_HH
#define WEBSOCKET_SERVER_SHARED_STATE_HH

#include "websocket_server/CommandLineInterface.hh"

#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace amadeus {
/// Forward declaration.
enum class StationId : std::uint8_t;

/// \brief Represents a 'notification' from a TCP connection, whenever a new
/// weather status update is received.
struct WeatherStatusNotification
{
    /// The unique station id which identifies the TCP connection.
    StationId id;
    /// The temperature read from the sensor.
    float temperature;
    /// The humidity read from the sensor.
    float humidity;
    /// The unix timestamp from when the sensor read the data.
    std::uint32_t time;
};

/// \brief Represents a simple WebSocketSession Context which is filled in by
/// the WebSocketSession itself whenever an asynchronous accept operation is
/// processed. This is done in order to register the callback for the
/// weather notifications.
template <typename SessionType>
struct WebSocketSessionCtx
{
    /// The callback whenever a new weather response is received from the TCP
    /// Session.
    using NotificationCallback = std::function<void(WeatherStatusNotification)>;
    /// Can either be PlainWebSocketSession or SSLWebSocketSession.
    SessionType* session;
    /// Called each time a new weather response was received from the TCP
    /// Session.
    NotificationCallback callback;
};

/// Forward declaration.
class PlainWebSocketSession;
class SSLWebSocketSession;
class PlainTCPSession;
class SSLTCPSession;
/// \brief This class is responsible for keeping track of all connected
/// websocket sessions. It provides functions for joining, leaving sessions and
/// sending messages to all websocket sessions. In essence, it simply holds
/// server data related to the websocket sessions and is thus shared by every
/// websocket session.
class SharedState
{
    using PlainWebSocketSessionCtx = WebSocketSessionCtx<PlainWebSocketSession>;
    using SSLWebSocketSessionCtx = WebSocketSessionCtx<SSLWebSocketSession>;

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

    /// \brief Returns the callback for the PlainWebSocketSession by a given
    /// UUID.
    /// \param _uuid The specific UUID bound to the PlainWebSocketSession.
    /// \remarks Not Thread-Safe. Must be called with a held lock.
    PlainWebSocketSessionCtx::NotificationCallback
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

    /// \brief Returns the callback for the SSLWebSocketSession by a given UUID.
    /// \param _uuid The specific UUID bound to the SSLWebSocketSession.
    /// \remarks Not Thread-Safe. Must be called with a held lock.
    SSLWebSocketSessionCtx::NotificationCallback
    findSSLWebSocketSession(boost::uuids::uuid const& _uuid)
    {
        if (auto const it = ssl_sessions_.find(_uuid);
            it != std::end(ssl_sessions_)) {
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

    /// \brief Finds a PlainTCPSession by a given stationId.
    /// \param _key The unique stationId.
    std::shared_ptr<PlainTCPSession> findPlainStation(StationId _key)
    {
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

    /// \brief Finds an SSLTCPSession by a given stationId.
    /// \param _key The unique stationId.
    std::shared_ptr<SSLTCPSession> findSSLStation(StationId _key)
    {
        if (auto const it = ssl_tcp_sessions_.find(_key);
            it != std::end(ssl_tcp_sessions_)) {
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

  public:
    /// A variant type for the PlainTCPSession & SSLTCPSession. This is
    /// necessary for returning multiple pointers to the TCP sessions
    /// (PlainTCPSession or SSLTCPSession) by a given stationId.
    using VariantType =
        std::variant<std::shared_ptr<PlainTCPSession>,
                     std::shared_ptr<SSLTCPSession>, std::monostate>;

    /// \brief Constructor.
    /// \param _docRoot The document resources directory.
    SharedState(std::string _docRoot, JSON const& _config);

    /// \brief Destructor.
    ~SharedState();

    /// \brief Returns the document root.
    std::string const& docRoot() const noexcept;

    /// \brief Returns the JSON config.
    JSON const& config() const noexcept;

    /// \brief Join a PlainWebSocketSession and insert it into the list.
    /// \param _uuid The UUID for the PlainWebSocketSession.
    /// \param _ctx The PlainWebSocketSessionCtx.
    /// \remarks Thread-Safe.
    bool join(boost::uuids::uuid _uuid,
              WebSocketSessionCtx<PlainWebSocketSession> _ctx);

    /// \brief Join an SSLWebSocketSession and insert it into the list.
    /// \param _uuid The UUID for the SSLWebSocketSession.
    /// \param _ctx The SSLWebSocketSessionCtx.
    /// \remarks Thread-Safe.
    bool join(boost::uuids::uuid _uuid,
              WebSocketSessionCtx<SSLWebSocketSession> _ctx);

    /// \brief Join a PlainTCPSession and insert it into the list.
    /// \param _id The StationId.
    /// \param _session The PlainTCPSession pointer.
    /// \remarks Thread-Safe.
    bool join(StationId _id, PlainTCPSession* _session);

    /// \brief Join a SSLTCPSession and insert it into the list.
    /// \param _id The StationId.
    /// \param _session The SSLTCPSession pointer.
    /// \remarks Thread-Safe.
    bool join(StationId _id, SSLTCPSession* _session);

    /// \brief Leaves a PlainWebSocketSession or SSLWebSocketSession by a given
    /// UUID.
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

    /// \brief Leaves a PlainTCPSession or SSLTCPSession by a given UUID.
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

    /// \brief Finds a WebSocketSession by a given SessionType
    /// (PlainWebSocketSession or SSLWebSocketSession) and UUID and returns the
    /// callback function for the weather status notification.
    /// \param _uuid The given UUID.
    template <typename SessionType>
    std::function<void(WeatherStatusNotification)>
    findWebSocketSession(boost::uuids::uuid const& _uuid)
    {
        std::scoped_lock<std::mutex> lk(mtx_);

        if constexpr (std::is_same_v<SessionType, PlainWebSocketSession>) {
            return findPlainWebSocketSession(_uuid);
        } else if constexpr (std::is_same_v<SessionType, SSLWebSocketSession>) {
            return findSSLWebSocketSession(_uuid);
        }
        return nullptr;
    }

    /// \brief Finds a specific PlainTCPSession or SSLTCPSession by a given
    /// stationId.
    /// \param _id The stationId.
    VariantType findStation(StationId _id)
    {
        auto sp = findPlainStation(_id);
        if (sp) {
            return sp;
        } else {
            auto sp = findSSLStation(_id);
            return sp;
        }
        return std::monostate();
    }

    /// \brief Returns all registered station ids as a vector.
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
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SHARED_STATE_HH
