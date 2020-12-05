/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

#include "watchdog/lib_api.hpp"
#include <iostream>
#include <filesystem>
#include <unordered_set>

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
    void remove(const std::filesystem::path& directory);

    /**
     *
     */
    std::string list() const;
};
