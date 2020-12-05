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
#include <sstream>
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
            std::ostringstream buffer;
            for (const auto& item : inventory.list())
            {
                buffer << item << "\n";
            }
            writer.write(buffer.str());
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
std::vector<std::string> read_lsof(const std::string& path)
{
    std::vector<std::string> output;
    std::array<char, 256> buffer;
    const auto cmd = fmt::format("lsof +D {}", path);
    std::ignore = path;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        spdlog::warn("failed to run lsof command");
        return {};
    }
    // skip header line in lsof output
    std::ignore = fgets(buffer.data(), buffer.size(), pipe.get());
    // iterate over lsof output line by line and add them as separate entries
    while (fgets(buffer.data(), buffer.size(), pipe.get())) {
        std::string entry(buffer.data());
        entry.pop_back();
        output.push_back(entry);
    }
    return output;
}

/**
 *
 */
void event_collector_lsof(
    Inventory& inventory,
    EventQueue& eventQueue)
{
    using namespace std::chrono_literals;
    while (true)
    {
        for (const auto& dir: inventory.list())
        {
            for (const auto& entry: read_lsof(dir))
            {
                std::istringstream iss(entry);
                std::vector<std::string> tokens(
                    std::istream_iterator<std::string>{iss},
                    std::istream_iterator<std::string>{});
                if (tokens.size() != 9)
                {
                    spdlog::warn("invalid lsof entry: {}, {}", tokens.size(), entry);
                    continue;
                }
                const auto pid = std::strtol(tokens.at(1).c_str(), nullptr, 10);
                eventQueue.push_item(std::make_unique<Event>(tokens.at(8), IN_OPEN,pid, tokens.at(0)));
            }
        }
        std::this_thread::sleep_for(1s);
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
    workers.push_back(std::thread(event_collector_lsof, std::ref(inventory), std::ref(eventQueue)));
    // workers.push_back(std::thread(event_collector_inotify, std::ref(eventQueue), std::ref(inventory)));
    // workers.push_back(std::thread(event_collector_poll, std::ref(eventQueue), std::ref(inventory)));

    std::for_each(workers.begin(), workers.end(), [](auto& t) { t.join(); });
}
