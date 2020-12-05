/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/asset_inventory.hpp"
#include <sstream>

/**
 *
 */
void AssetInventory::add(const std::filesystem::path& directory)
{
    for(const auto& filepath: std::filesystem::recursive_directory_iterator(directory))
    {
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
        assets.erase(filepath.path().string());
    }
}

/**
 *
 */
void AssetInventory::clear()
{
    assets.clear();
}

/**
 *
 */
std::string AssetInventory::list() const
{
    std::ostringstream buffer;
    for (const auto& asset: assets)
    {
        buffer << asset << "\n";
    }
    return buffer.str();
}
