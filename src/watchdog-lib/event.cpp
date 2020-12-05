/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/event.hpp"
#include "fmt/format.h"
#include <cstdint>
#include <optional>

using namespace watchdog;

/**
 *
 */
Event::Event(
    const std::string& filepath,
    const std::uint32_t event_type)
    : filepath(filepath)
    , event_type(event_type)
{
}

/**
 *
 */
Event::Event(
    const std::string& filepath,
    const std::uint32_t event_type,
    std::optional<pid_t> process_pid,
    std::optional<std::string> process_path)
    : filepath(filepath)
    , event_type(event_type)
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
        actor = fmt::format("process {:<30} (pid: {d:>5})", process_path.value(), process_pid.value());
    }
    return fmt::format("{:<40} {:<10} \"{}\"\n", actor, action, filepath);
}
