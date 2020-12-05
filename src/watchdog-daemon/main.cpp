/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "spdlog/spdlog.h"
#include "watchdog/asset_inventory.hpp"
#include "watchdog/named_pipe.hpp"
#include "watchdog/queue.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

using NamedPipeMessageQueue = watchdog::Queue<NamedPipeMessage>;

/**
 * Listens to a dedicated named pipe for new directories to watch.
 */
void named_pipe_reader(NamedPipeMessageQueue& messageQueue)
{
    using namespace std::chrono_literals;
    NamedPipeReader reader("/tmp/watchdog/sin");
    while (true)
    {
        const auto& content = reader.read();

        // if there is nothing to read, we have nothing to do

        if (content.empty())
        {
            std::this_thread::sleep_for(500ms);
            continue;
        }

        // deserialize message

        spdlog::debug("received command: {}", content);
        auto message = std::make_unique<NamedPipeMessage>(content);
        if (!message->valid())
        {
            spdlog::warn("invalid command: {}", content);
            continue;
        }

        // push message into queue for async processing

        messageQueue.push_item(std::move(message));
    }
}

/**
 *
 */
void inventory_manager(NamedPipeMessageQueue& messageQueue, AssetInventory& inventory)
{
    NamedPipeWriter writer("/tmp/watchdog/sout", O_RDWR);
    while (true)
    {
        auto asset = messageQueue.pop_item();
        if (asset->message_type == "a")
        {
            inventory.add(asset->message_args.value());
            writer.write("a,OK");
        }
        if (asset->message_type == "c")
        {
            inventory.clear();
            writer.write("c,OK");
        }
        if (asset->message_type == "l")
        {
            writer.write(inventory.list());
            writer.write("l,OK");
        }
        spdlog::info("received task ({}): {}", asset->message_type, asset->message_args.value_or(""));
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
    workers.push_back(std::thread(named_pipe_reader, std::ref(messageQueue)));
    workers.push_back(std::thread(inventory_manager, std::ref(messageQueue), std::ref(inventory)));

    std::for_each(workers.begin(), workers.end(), [](auto& t) { t.join(); });
}
