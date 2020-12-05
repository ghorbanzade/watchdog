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
    : _path(pipe_path.string().c_str())
{
    mkfifo(_path, 666);
    _fd = open(_path, O_WRONLY);
    spdlog::debug("created named pipe");
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
    if (!_fd && std::filesystem::exists(_path))
    {
        mkfifo(_path, 666);
        _fd = open(_path, O_WRONLY);
        spdlog::info("created named pipe");
        // @todo handle case if open fails (_fd == -1)
    }
    const auto content_size = ::write(_fd, &message[0], message.size() + 1);
    if (content_size < 0)
    {
        spdlog::warn("failed to write to named pipe");
    }
}
