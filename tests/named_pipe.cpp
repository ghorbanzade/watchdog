/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/named_pipe.hpp"
#include "catch2/catch.hpp"

using namespace watchdog;

TEST_CASE("NamedPipeMessage")
{
    SECTION("should not throw on corrupt data")
    {
        for (const auto content : { "", "q", "w", "hello" })
        {
            CHECK_NOTHROW(NamedPipeMessage(content));
            NamedPipeMessage msg(content);
            CHECK(!msg.valid());
        }
    }
    SECTION("deserialize messages")
    {
        for (const auto content : { "a", "a,,", "a,/var/log" })
        {
            CHECK_NOTHROW(NamedPipeMessage(content));
            NamedPipeMessage msg(content);
            CHECK(msg.valid());
            CHECK(msg.message_type == "a");
        }
    }
}
