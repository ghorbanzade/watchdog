/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

/**
 * @file event.hpp
 *
 * @author Pejman Ghorbanzade <pejman@ghorbanzade.com>
 * @date 2020
 */

#include "watchdog/lib_api.hpp"
#include <optional>

namespace watchdog {

    /**
     *
     */
    struct WATCHDOG_API Event
    {
    private:
        std::string filepath;
        uint32_t event_type;
        std::optional<pid_t> process_pid;
        std::optional<std::string> process_path;

    public:
        /**
         *
         */
        Event(const std::string& filepath, const uint32_t event_type);

        /**
         *
         */
        Event(const std::string& filepath, const uint32_t event_type, std::optional<pid_t> process_pid, std::optional<std::string> process_path);

        /**
         *
         */
        std::string string() const;
    };

} // namespace watchdog
