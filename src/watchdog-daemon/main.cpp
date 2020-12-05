/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "spdlog/spdlog.h"
#include "watchdog/event.hpp"
#include "watchdog/inventory.hpp"
#include "watchdog/named_pipe.hpp"
#include "watchdog/queue.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <sys/inotify.h>
#include <thread>

using namespace watchdog;
using EventQueue = Queue<Event>;
using NamedPipeMessageQueue = Queue<NamedPipeMessage>;

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
void inventory_manager(
    NamedPipeMessageQueue& messageQueue,
    Inventory& inventory,
    EventQueue& eventQueue)
{
    NamedPipeWriter writer("/tmp/watchdog/sout", O_RDWR);
    while (true)
    {
        const auto& msg = messageQueue.pop_item();
        spdlog::info("received task ({}): {}", msg->message_type, msg->message_args.value_or(""));
        if (msg->message_type == "a")
        {
            inventory.add(msg->message_args.value());
            writer.write("a,OK");
        }
        if (msg->message_type == "c")
        {
            inventory.clear();
            writer.write("c,OK");
        }
        if (msg->message_type == "l")
        {
            writer.write(inventory.list());
            writer.write("l,OK");
        }
        if (msg->message_type == "m")
        {
            while (eventQueue.has_item())
            {
                const auto event = eventQueue.pop_item();
                writer.write(event->string());
            }
            writer.write("m,OK");
        }
    }
}

/**
 *
 */
void event_collector_basic(
    Inventory& inventory,
    EventQueue& eventQueue)
{
    std::ignore = inventory;
    while (true)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5s);
        eventQueue.push_item(std::make_unique<Event>("some-file", IN_OPEN));
    }
}

/**
 *
 */
int main()
{
    Inventory inventory;
    EventQueue eventQueue;
    NamedPipeMessageQueue messageQueue;

    std::vector<std::thread> workers;
    workers.push_back(std::thread(named_pipe_reader, std::ref(messageQueue)));
    workers.push_back(std::thread(inventory_manager, std::ref(messageQueue), std::ref(inventory), std::ref(eventQueue)));
    workers.push_back(std::thread(event_collector_basic, std::ref(inventory), std::ref(eventQueue)));
    // workers.push_back(std::thread(event_collector_lsof, std::ref(eventQueue), std::ref(inventory)));
    // workers.push_back(std::thread(event_collector_inotify, std::ref(eventQueue), std::ref(inventory)));

    std::for_each(workers.begin(), workers.end(), [](auto& t) { t.join(); });
}
