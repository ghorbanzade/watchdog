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

using namespace watchdog;

/**
 *
 */
NamedPipeReader::NamedPipeReader(const std::filesystem::path& path, const int mode)
    : _path(path.string())
{
    std::filesystem::create_directories(path.parent_path());
    mkfifo(_path.c_str(), 666);
    if ((_fd = open(_path.c_str(), mode)) == -1)
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
NamedPipeWriter::NamedPipeWriter(const std::filesystem::path& path, const int mode)
    : _path(path.string())
{
    std::filesystem::create_directories(path.parent_path());
    mkfifo(_path.c_str(), 666);
    if ((_fd = open(_path.c_str(), mode)) == -1)
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
    : message_type("")
    , message_args(std::nullopt)
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
    const auto& modes = { "a", "l", "c", "m" };
    const auto& it = std::find(modes.begin(), modes.end(), tokens.at(0));
    if (modes.end() != it)
    {
        message_type = *it;
    }
    if (1 < tokens.size())
    {
        message_args = tokens.at(1);
    }
}
