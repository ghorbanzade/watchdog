/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/watchdog.hpp"
#include "catch2/catch.hpp"

TEST_CASE("sample-case")
{
    SECTION("sample-section")
    {
        CHECK(watchdog::get_watchdog() == 42);
    }
}
