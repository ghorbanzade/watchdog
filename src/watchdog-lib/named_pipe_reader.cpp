/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/named_pipe_reader.hpp"
#include "spdlog/spdlog.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 *
 */
NamedPipeReader::NamedPipeReader(const std::filesystem::path& pipe_path)
    : _path(pipe_path.string())
{
    std::filesystem::create_directories(pipe_path.parent_path());
    mkfifo(_path.c_str(), 666);
    if ((_fd = open(_path.c_str(), O_RDONLY)) == -1)
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
