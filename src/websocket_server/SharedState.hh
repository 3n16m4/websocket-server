#ifndef WEBSOCKET_SERVER_SHARED_STATE_HH
#define WEBSOCKET_SERVER_SHARED_STATE_HH

#include "websocket_server/CommandLineInterface.hh"

#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <vector>

/// TODO: Join TCP Connections here.
namespace amadeus {
/// \brief This class is responsible for keeping track of all connected
/// websocket sessions. It provides functions for joining, leaving sessions and
/// sending messages to all websocket sessions. In essence, it simply holds
/// server data related to the websocket sessions and is thus shared by every
/// websocket session.
class PlainWebSocketSession;
class SSLWebSocketSession;
class PlainTCPSession;
class SSLTCPSession;
enum class StationId : std::uint8_t;
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
    std::unordered_set<PlainWebSocketSession*> plain_sessions_;
    /// A set to track all ssl websockets.
    std::unordered_set<SSLWebSocketSession*> ssl_sessions_;
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
                _sequence.emplace_back(weak_from_this(session));
            }
        } else if constexpr (std::is_same_v<SessionType, SSLWebSocketSession>) {
            _sequence.reserve(ssl_sessions_.size());
            for (auto* session : ssl_sessions_) {
                _sequence.emplace_back(weak_from_this(session));
            }
        }
    }

  public:
    /// \brief Constructor.
    /// \param _docRoot The document resources directory.
    explicit SharedState(std::string _docRoot, JSON const& _config);

    /// \brief Returns the document root.
    std::string const& docRoot() const noexcept;

	/// \brief Returns the JSON config.
	JSON const& config() const noexcept;

    /// \brief Join a PlainWebSocketSession and insert it into the list.
    /// \param _session The PlainWebSocketSession pointer.
    /// \remarks Thread-Safe.
    void join(PlainWebSocketSession* _session);

    /// \brief Join an SSLWebSocketSession and insert it into the list.
    /// \param _session The SSLWebSocketSession pointer.
    /// \remarks Thread-Safe.
    void join(SSLWebSocketSession* _session);

    /// \brief Join a PlainTCPSession and insert it into the lsit.
    /// \param _session The PlainTCPSession pointer.
    /// \remarks Thread-Safe.
    void join(StationId _id, PlainTCPSession* _session);
	
    /// \brief Join a SSLTCPSession and insert it into the lsit.
    /// \param _session The SSLTCPSession pointer.
    /// \remarks Thread-Safe.
    void join(StationId _id, SSLTCPSession* _session);

    /// \brief Leave a PlainWebSocketSession and erase it from the list.
    /// \param _session The PlainWebSocketSession pointer.
    /// \remarks Thread-Safe.
    void leave(PlainWebSocketSession* _session);

    /// \brief Leave an SSLWebSocketSession and erase it from the list.
    /// \param _session The SSLWebSocketSession pointer.
    /// \remarks Thread-Safe.
    void leave(SSLWebSocketSession* _session);

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
