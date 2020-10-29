#include <gtest/gtest.h>

#include "websocket_server/SharedState.hh"

using namespace amadeus;

TEST(SharedStateTest, JoinPlain)
{
    auto state = std::make_shared<SharedState>();
    auto sessionPlain = std::make_shared<WebsocketSessionPlain>();

    state->join(sessionPlain.get());
    state->join(sessionPlain.get());

    ASSERT_TRUE(state->size<WebsocketSessionPlain>() == 1);
    ASSERT_TRUE(state->size<WebsocketSessionSSL>() == 0);
}

TEST(SharedStateTest, JoinSSL)
{
    auto state = std::make_shared<SharedState>();
    auto sessionSSL = std::make_shared<WebsocketSessionSSL>();

    state->join(sessionSSL.get());
    state->join(sessionSSL.get());

    ASSERT_TRUE(state->size<WebsocketSessionSSL>() == 1);
    ASSERT_TRUE(state->size<WebsocketSessionPlain>() == 0);
}