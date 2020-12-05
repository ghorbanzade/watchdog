/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "named_pipe_reader.hpp"
#include "spdlog/spdlog.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 *
 */
NamedPipeReader::NamedPipeReader(const std::filesystem::path& pipe_path)
    : _path(pipe_path.string().c_str())
{
    mkfifo(_path, 666);
    _fd = open(_path, O_RDONLY);
    spdlog::info("created named pipe");
}

/**
 * Destroys resources belonging to the named pipe if they were ever allocated.
 */
NamedPipeReader::~NamedPipeReader()
{
    if (!_fd) {
        close(_fd);
        unlink(_path);
        spdlog::info("removed named pipe");
    }
}

/**
 *
 */
std::string NamedPipeReader::read()
{
    if (!_fd && std::filesystem::exists(_path))
    {
        mkfifo(_path, 666);
        _fd = open(_path, O_RDONLY);
        spdlog::info("created named pipe");
        // @todo handle case if open fails (_fd == -1)
    }
    const auto content_size = ::read(_fd, _buffer, sizeof(_buffer));
    if (content_size < 0) {
        spdlog::warn("failed to read from named pipe");
        return {};
    }
    if (content_size == 0) {
        return {};
    }
    return std::string(_buffer, content_size - 1);
}
