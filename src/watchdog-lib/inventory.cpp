/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/inventory.hpp"

using namespace watchdog;

/**
 *
 */
void Inventory::add(const std::filesystem::path& directory)
{
    std::unique_lock<std::mutex> lock(_mutex);
    const auto wasEmpty = _items.empty();
    _items.insert(directory.string());
    lock.unlock();
    if (wasEmpty)
    {
        _cv.notify_one();
    }
}

/**
 *
 */
void Inventory::clear()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _items.clear();
}

/**
 *
 */
std::unordered_set<std::string> Inventory::list()
{
    std::unique_lock<std::mutex> lock(_mutex);
    return _items;
}
