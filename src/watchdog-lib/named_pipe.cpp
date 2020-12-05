/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/named_pipe.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 *
 */
NamedPipeReader::NamedPipeReader(const std::filesystem::path& pipe_path, const int flags)
    : _path(pipe_path.string())
{
    std::filesystem::create_directories(pipe_path.parent_path());
    mkfifo(_path.c_str(), 666);
    if ((_fd = open(_path.c_str(), flags)) == -1)
    {
        spdlog::error("failed to create named pipe at {}", _path);
        return;
    }
    spdlog::info("created named pipe at {}", _path);
}

/**
 * Destroys resources belonging to the named pipe if they were ever allocated.
 */
NamedPipeReader::~NamedPipeReader()
{
    if (_fd)
    {
        close(_fd);
        spdlog::info("removed named pipe");
    }
}

/**
 *
 */
std::string NamedPipeReader::read()
{
    const auto content_size = ::read(_fd, _buffer, sizeof(_buffer));
    if (content_size < 0)
    {
        spdlog::warn("failed to read from named pipe");
        return "";
    }
    if (content_size == 0)
    {
        return "";
    }
    return std::string(_buffer, content_size - 1);
}

/**
 *
 */
bool NamedPipeReader::expect_response(const std::string& expected)
{
    const auto& response = read();
    spdlog::debug("received reply: {}", response);
    return response == expected;
}

/**
 *
 */
NamedPipeWriter::NamedPipeWriter(const std::filesystem::path& pipe_path, const int flags)
    : _path(pipe_path.string())
{
    std::filesystem::create_directories(pipe_path.parent_path());
    mkfifo(_path.c_str(), 666);
    if ((_fd = open(_path.c_str(), flags)) == -1)
    {
        spdlog::error("failed to create named pipe at {}", _path);
        return;
    }
    spdlog::info("created named pipe at {}", _path);
}

/**
 * Destroys resources belonging to the named pipe if they were ever allocated.
 */
NamedPipeWriter::~NamedPipeWriter()
{
    if (_fd)
    {
        close(_fd);
        spdlog::debug("removed named pipe");
    }
}

/**
 *
 */
void NamedPipeWriter::write(const std::string& message)
{
    const auto content_size = ::write(_fd, &message[0], message.size() + 1);
    if (content_size < 0)
    {
        spdlog::warn("failed to write to named pipe");
    }
}

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
