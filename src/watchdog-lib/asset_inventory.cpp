/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/asset_inventory.hpp"
#include <iostream>

/**
 *
 */
void AssetInventory::add(const std::filesystem::path& directory)
{
    for(const auto& filepath: std::filesystem::recursive_directory_iterator(directory))
    {
        std::cout << "adding: " << filepath << std::endl;
        assets.insert(filepath.path().string());
    }
}

/**
 *
 */
void AssetInventory::remove(const std::filesystem::path& directory)
{
    for(const auto& filepath: std::filesystem::recursive_directory_iterator(directory))
    {
        std::cout << "removing: " << filepath << std::endl;
        assets.erase(filepath.path().string());
    }
}

/**
 *
 */
std::string AssetInventory::list() const
{
    return "hello";
}
