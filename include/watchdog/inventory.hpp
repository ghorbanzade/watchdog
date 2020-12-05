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
#include <iostream>
#include <unordered_set>

namespace watchdog {

    /**
     *
     */
    class WATCHDOG_API Inventory
    {
        std::unordered_set<std::string> items;

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
        std::string list() const;
    };

} // namespace watchdog
