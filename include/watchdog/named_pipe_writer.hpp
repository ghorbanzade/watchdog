/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

#include <filesystem>
#include "watchdog/named_pipe_message.hpp"

/**
 *
 */
class NamedPipeWriter
{
    const char* _path; /**< full path to the named pipe */
    char _buffer[1024]; /**< buffer to read messages from */
    mutable int _fd; /**< descriptor for the named pipe */
public:
    /**
     *
     */
    NamedPipeWriter(const std::filesystem::path& pipe_path);

    /**
     * Destroys named pipe resources if they were ever allocated.
     */
    ~NamedPipeWriter();

    /**
     *
     */
    void write(const std::string& message);
};