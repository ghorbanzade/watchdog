/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "spdlog/spdlog.h"
#include "watchdog/watchdog.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

#include "watchdog/asset_inventory.hpp"
#include "watchdog/named_pipe_message.hpp"
#include "watchdog/named_pipe_reader.hpp"

/**
 * Listens to a dedicated named pipe for new directories to watch.
 */
void named_pipe_listener(NamedPipeMessageQueue& messageQueue)
{
    using namespace std::chrono_literals;
    NamedPipeReader reader("/tmp/watchdog");
    while (true)
    {
        const auto& content = reader.read();

        // if there is nothing to read, we have nothing to do
        if (content.empty())
        {
            std::this_thread::sleep_for(2s);
            continue;
        }

        spdlog::info("received command: {}", content);
        auto message = NamedPipeMessage::deserialize(content);
        if (!message)
        {
            spdlog::warn("invalid command: {}", content);
            continue;
        }
        messageQueue.push_message(std::move(message));
    }
}

/**
 *
 */
void inventory_manager(NamedPipeMessageQueue& messageQueue)
{
    while (true)
    {
        auto asset = messageQueue.pop_message();
        std::cout << fmt::format("received task: {} ({})", asset->_filepath.value_or(""), asset->_mode) << std::endl;
    }
}

/**
 *
 */
int main()
{
    AssetInventory inventory;
    NamedPipeMessageQueue messageQueue;

    std::vector<std::thread> workers;
    workers.push_back(std::thread(named_pipe_listener, std::ref(messageQueue)));
    workers.push_back(std::thread(inventory_manager, std::ref(messageQueue)));

    std::for_each(workers.begin(), workers.end(), [](auto& t) { t.join(); });
}
