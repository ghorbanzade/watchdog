/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

/**
 * @file named_pipe.hpp
 *
 * @brief Utility classes for interacting with Named Pipes.
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
    class WATCHDOG_API AssetInventory
    {
        std::unordered_set<std::string> assets;

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
