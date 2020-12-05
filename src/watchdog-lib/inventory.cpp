/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/inventory.hpp"
#include <sstream>

using namespace watchdog;

/**
 *
 */
void Inventory::add(const std::filesystem::path& directory)
{
    items.insert(directory.string());
}

/**
 *
 */
void Inventory::clear()
{
    items.clear();
}

/**
 *
 */
std::string Inventory::list() const
{
    std::ostringstream buffer;
    for (const auto& item : items)
    {
        buffer << item << "\n";
    }
    return buffer.str();
}
