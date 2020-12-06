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
#include <cstdint>
#include <optional>
#include <string>

namespace watchdog {

    /**
     *
     */
    struct WATCHDOG_API Event
    {
    private:
        std::string filepath;
        std::optional<pid_t> process_pid;
        std::optional<std::string> process_path;

    public:
        /**
         *
         */
        Event(const std::string& filepath);

        /**
         *
         */
        Event(const std::string& filepath, std::optional<pid_t> process_pid, std::optional<std::string> process_path);

        /**
         *
         */
        std::string string() const;
    };

} // namespace watchdog
