/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/event.hpp"
#include "fmt/format.h"
#include <optional>

using namespace watchdog;

/**
 *
 */
Event::Event(const std::string& filepath)
    : filepath(filepath)
{
}

/**
 *
 */
Event::Event(
    const std::string& filepath,
    std::optional<pid_t> process_pid,
    std::optional<std::string> process_path)
    : filepath(filepath)
    , process_pid(process_pid)
    , process_path(process_path)
{
}

/**
 *
 */
std::string Event::string() const
{
    return fmt::format("{:<10} {:>7} {:<30}\n",
        process_path.value_or("-"), process_pid.value_or(-1), filepath);
}
