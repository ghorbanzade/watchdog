/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/named_pipe_message.hpp"
#include <iostream>
#include <string>

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

/**
 *
 */
std::unique_ptr<NamedPipeMessage> NamedPipeMessage::deserialize(
    const std::string& message)
{
    std::string buf;
    std::stringstream ss(message);
    std::vector<std::string> tokens;
    while (std::getline(ss, buf, ','))
    {
        tokens.push_back(buf);
    }
    if (tokens.size() == 0)
    {
        return nullptr;
    }
    if (tokens.size() == 1)
    {
        return std::make_unique<NamedPipeMessage>(tokens.at(0), "");
    }
    return std::make_unique<NamedPipeMessage>(tokens.at(0), tokens.at(1));
}
