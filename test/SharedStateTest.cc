#include <gtest/gtest.h>

#include "websocket_server/SharedState.hh"

using namespace amadeus;

TEST(SharedStateTest, JoinSSLWebSocketSession)
{
    auto state = std::make_shared<SharedState>();
    auto sessionPlain = std::make_shared<SSLWebSocketSession>();

    state->join(sessionPlain.get());
    state->join(sessionPlain.get());

    ASSERT_TRUE(state->size<SSLWebSocketSession>() == 1);
    ASSERT_TRUE(state->size<SSLWebSocketSession>() == 0);
}

TEST(SharedStateTest, JoinSSLWebsocketSession)
{
    auto state = std::make_shared<SharedState>();
    auto sessionSSL = std::make_shared<SSLWebSocketSession>();

    state->join(sessionSSL.get());
    state->join(sessionSSL.get());

    ASSERT_TRUE(state->size<SSLWebSocketSession>() == 1);
    ASSERT_TRUE(state->size<SSLWebSocketSession>() == 0);
}