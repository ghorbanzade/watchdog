/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <filesystem>

/**
 *
 */
class AssetQueue
{
    std::mutex _mutex;
    std::condition_variable _cv;
    std::queue<std::filesystem::path> _queue;
public:

    /**
     *
     */
    void add_asset(const std::filesystem::path& assetDir);

    /**
     *
     */
    std::filesystem::path take_asset();

};