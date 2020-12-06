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
Event::Event(const std::string& filepath) : filepath(filepath)
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
    const auto action = "accessed";
    std::string actor = "unidentified process";
    if (process_pid && process_path)
    {
        actor = fmt::format("process {} (pid: {})", process_path.value(), process_pid.value());
    }
    return fmt::format("{} {} \"{}\"\n", actor, action, filepath);
}
