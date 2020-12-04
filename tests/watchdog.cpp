/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "catch2/catch.hpp"
#include "watchdog/watchdog.hpp"

TEST_CASE("sample-case")
{
    SECTION("sample-section")
    {
        CHECK(watchdog::get_watchdog() == 42);
    }
}
