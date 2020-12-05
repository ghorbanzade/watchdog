/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/named_pipe_message.hpp"

/**
 *
 */
void AssetQueue::add_asset(const std::filesystem::path& assetDir)
{
    std::unique_lock<std::mutex> lock(_mutex);
    auto wasEmpty = _queue.empty();
    _queue.push(assetDir);
    lock.unlock();
    if (wasEmpty) {
        _cv.notify_one();
    }
}

/**
 *
 */
std::filesystem::path AssetQueue::take_asset()
{
    std::unique_lock<std::mutex> lock(_mutex);
    while (_queue.empty())
    {
        _cv.wait(lock);
    }
    auto asset = _queue.front();
    _queue.pop();
    return asset;
}
