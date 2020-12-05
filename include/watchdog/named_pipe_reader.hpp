/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

#include <filesystem>

/**
 *
 */
class NamedPipeReader
{
    const std::string _path; /**< full path to the named pipe */
    char _buffer[1024]; /**< buffer to read messages from */
    mutable int _fd; /**< descriptor for the named pipe */
public:
    /**
     *
     */
    NamedPipeReader(const std::filesystem::path& pipe_path);

    /**
     * Destroys named pipe resources if they were ever allocated.
     */
    ~NamedPipeReader();

    /**
     *
     */
    std::string read();
};