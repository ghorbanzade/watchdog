/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

/**
 * @file inventory.hpp
 *
 * @author Pejman Ghorbanzade <pejman@ghorbanzade.com>
 * @date 2020
 */

#include "watchdog/lib_api.hpp"
#include <filesystem>
#include <condition_variable>
#include <mutex>
#include <unordered_set>

namespace watchdog {

    /**
     *
     */
    class WATCHDOG_API Inventory
    {
        std::mutex _mutex;
        std::condition_variable _cv;
        std::unordered_set<std::string> _items;

    public:
        /**
         *
         */
        void add(const std::filesystem::path& directory);

        /**
         *
         */
        void clear();

        /**
         *
         */
        std::unordered_set<std::string> list();
    };

} // namespace watchdog
