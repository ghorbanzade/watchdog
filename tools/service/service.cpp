/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/watchdog.hpp"
#include "spdlog/spdlog.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <filesystem>

/**
 *
 */
int main()
{
    using namespace std::chrono_literals;
    auto index = 0u;
    while (true) {
        std::filesystem::path home("/home/pejman");
        std::this_thread::sleep_for(2s);
        spdlog::info("service ready for command {}", index);
        std::cout << home.string() << watchdog::get_watchdog() << std::endl;
        index++;
    }
}
