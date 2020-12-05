/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/named_pipe_message.hpp"
#include <iostream>
#include <string>
#include <algorithm>

/**
 *
 */
NamedPipeMessage::NamedPipeMessage(const std::string& message)
    : _mode(""), _filepath(std::nullopt)
{
    std::string buf;
    std::stringstream ss(message);
    std::vector<std::string> tokens;
    while (std::getline(ss, buf, ','))
    {
        tokens.push_back(buf);
    }
    if (tokens.empty())
    {
        return;
    }
    const auto& modes = { "a", "r", "l", "c", "m" };
    const auto& it = std::find(modes.begin(), modes.end(), tokens.at(0));
    if (modes.end() != it)
    {
        _mode = *it;
    }
    if (1 < tokens.size())
    {
        _filepath = tokens.at(1);
    }
}

/**
 *
 */
void NamedPipeMessageQueue::push_message(std::unique_ptr<NamedPipeMessage> assetDir)
{
    std::unique_lock<std::mutex> lock(_mutex);
    auto wasEmpty = _queue.empty();
    _queue.push(std::move(assetDir));
    lock.unlock();
    if (wasEmpty)
    {
        _cv.notify_one();
    }
}

/**
 *
 */
std::unique_ptr<NamedPipeMessage> NamedPipeMessageQueue::pop_message()
{
    std::unique_lock<std::mutex> lock(_mutex);
    while (_queue.empty())
    {
        _cv.wait(lock);
    }
    auto asset = std::move(_queue.front());
    _queue.pop();
    return asset;
}
