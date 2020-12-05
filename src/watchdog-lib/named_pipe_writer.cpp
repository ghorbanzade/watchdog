/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/named_pipe_writer.hpp"
#include "spdlog/spdlog.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 *
 */
NamedPipeWriter::NamedPipeWriter(const std::filesystem::path& pipe_path)
    : _path(pipe_path.string())
{
    std::filesystem::create_directories(pipe_path.parent_path());
    mkfifo(_path.c_str(), 666);
    if ((_fd = open(_path.c_str(), O_WRONLY)) == -1)
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
