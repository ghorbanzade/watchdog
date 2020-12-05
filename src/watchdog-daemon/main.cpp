/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "spdlog/spdlog.h"
#include "watchdog/watchdog.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

#include "watchdog/named_pipe_message.hpp"
#include "watchdog/named_pipe_reader.hpp"

/**
 * Listens to a dedicated named pipe for new directories to watch.
 */
void listen_for_asset_directories(
    const std::filesystem::path& pipePath,
    AssetQueue& assetQueue)
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
        if (content == "exit")
        {
            spdlog::warn("no longer listening for asset directories");
            break;
        }
        std::filesystem::path assetEntry(content);
        if (!std::filesystem::is_directory(assetEntry))
        {
            spdlog::warn("asset directory is invalid");
            continue;
        }
        assetQueue.add_asset(assetEntry);
    }
}

/**
 *
 */
void monitor_assets(AssetQueue& assetQueue)
{
    while (true)
    {
        auto asset = assetQueue.take_asset();
        std::cout << "Got some work: " << asset << std::endl;
    }
}

/**
 *
 */
int main()
{
    AssetQueue assetQueue;
    std::thread pipe_listener(listen_for_asset_directories, "/tmp/watchdog", std::ref(assetQueue));
    std::thread asset_monitor(monitor_assets, std::ref(assetQueue));
    pipe_listener.join();
    asset_monitor.join();
}
