/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "spdlog/spdlog.h"
#include "watchdog/watchdog.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>
#include <iostream>

#include "watchdog/named_pipe_message.hpp"
#include "watchdog/named_pipe_reader.hpp"

/**
 * Listens to a dedicated named pipe for new directories to watch.
 */
void listen_for_asset_directories(
    const std::filesystem::path& pipePath,
    NamedPipeMessageQueue& NamedPipeMessageQueue)
{
    using namespace std::chrono_literals;
    NamedPipeReader reader(pipePath);
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
        auto query = NamedPipeMessage::deserialize(content);
        if (!query)
        {
            spdlog::warn("invalid command: {}", content);
            continue;
        }
        NamedPipeMessageQueue.add_asset(std::move(query));
    }
}

/**
 *
 */
void monitor_assets(NamedPipeMessageQueue& NamedPipeMessageQueue)
{
    while (true)
    {
        auto asset = NamedPipeMessageQueue.take_asset();
        std::cout << fmt::format("received task: {} ({})", asset->_filepath.value_or(""), asset->_mode) << std::endl;
    }
}

/**
 *
 */
int main()
{
    NamedPipeMessageQueue NamedPipeMessageQueue;
    std::thread pipe_listener(listen_for_asset_directories, "/tmp/watchdog", std::ref(NamedPipeMessageQueue));
    std::thread asset_monitor(monitor_assets, std::ref(NamedPipeMessageQueue));
    pipe_listener.join();
    asset_monitor.join();
}
