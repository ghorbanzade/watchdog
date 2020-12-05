/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

#include "watchdog/lib_api.hpp"
#include <condition_variable>
#include <fcntl.h>
#include <filesystem>
#include <mutex>
#include <optional>
#include <queue>

/**
 * @file named_pipe.hpp
 *
 * @brief Utility classes for interacting with Named Pipes.
 *
 * @author Pejman Ghorbanzade <pejman@ghorbanzade.com>
 * @date 2020
 */

/**
 * @brief Reads multiple independent messages from a given Named Pipe.
 *
 * @details This class provides a mechanism for the watchdog daemon to
 *          receive multiple separate requests through a dedicated Named
 *          Pipe and for the watchdog command-line tool to check for
 *          responses to its requests.
 */
class NamedPipeReader
{
    const std::string _path; /**< full path to the named pipe */
    char _buffer[1024]; /**< buffer to read messages from */
    int _fd; /**< descriptor for the named pipe */
public:
    /**
     * @brief Registers a given file as a NamedPipe to be read in
     *        subsequent calls to the member functions.
     *
     * @details Named pipe is acquired as a resource and let go at destruction
     *          time when this object goes out of scope (RAII).
     *
     * @param path full path to the Named Pipe.
     * @param flags flags to be used when opening the Named Pipe.
     *
     * @todo Consider adding O_CREAT to handle first-time cases when the pipe
     *       does not exist yet.
     */
    NamedPipeReader(const std::filesystem::path& path, const int flags = O_RDONLY);

    /**
     * Destroys named pipe resources if they were ever allocated.
     */
    ~NamedPipeReader();

    /**
     *
     */
    std::string read();

    /**
     * @brief Checks and logs if the response to be read from the Named Pipe
     *        matches our expectation.
     *
     * @details This method is provided for convenience and internally uses
     *          `read` to read contents of the Named Pipe. There is no
     *          guarantee that the content is a single line.
     *
     * @param expected message expected to be received as response.
     *
     * @returns true if received response matches our expectation.
     */
    bool expect_response(const std::string& expected);
};

/**
 * @brief Writes multiple independent messages to a given Named Pipe.
 *
 * @details This class provides a mechanism for the watchdog command-line
 *          tool to submit multiple separate requests through a dedicated
 *          Named Pipe and for the watchdog daemon to provide responses to
 *          those requests asynchronously.
 */
class NamedPipeWriter
{
    const std::string _path; /**< full path to the named pipe */
    char _buffer[1024]; /**< buffer to read messages from */
    int _fd; /**< descriptor for the named pipe */
public:
    /**
     * @brief Registers a given file as a NamedPipe to be written into in
     *        subsequent calls to the member functions.
     *
     * @details Named pipe is acquired as a resource and let go at destruction
     *          time when this object goes out of scope (RAII).
     *
     * @param path full path to the Named Pipe.
     * @param flags Flags to be used when opening the Named Pipe.
     *
     * @todo Consider adding O_CREAT to handle first-time cases when the pipe
     *       does not exist yet.
     */
    NamedPipeWriter(const std::filesystem::path& path, const int flags = O_WRONLY);

    /**
     * @details cleans up the acquired named pipe resource if it was ever allocated.
     */
    ~NamedPipeWriter();

    /**
     * @brief Writes a given message into the Named Pipe.
     *
     * @details This operation is blocking but we do not wait to receive an
     *          acknowledgement from the reader.
     *
     * @param message content to be written into the Named Pipe.
     */
    void write(const std::string& message);
};

/**
 * @brief Represents any "Request" message written into a Named Pipe.
 *
 * @details Instances of this class repesent messages initiated by the
 *          Watchdog command-line tool that are written into a Named Pipe
 *          and read by the Watchdog daemon. This class is meant to contain
 *          the logic for serializing and later deserializing these requests.
 *
 * @todo In more serious applications, a more extensible and easy to maintain
 *       serialization mechanism using a known schema should be used.
 *       Flatbuffers, Protobuf, or MessagePack are a few candidate technologies
 *       for proper serialization.
 */
struct WATCHDOG_API NamedPipeMessage
{
    std::string _mode;
    std::optional<std::string> _filepath;

    /**
     *
     */
    NamedPipeMessage(const std::string& message);

    /**
     *
     */
    inline bool valid() const { return !_mode.empty(); }
};

/**
 *
 */
class WATCHDOG_API NamedPipeMessageQueue
{
    std::mutex _mutex;
    std::condition_variable _cv;
    std::queue<std::unique_ptr<NamedPipeMessage>> _queue;

public:
    /**
     *
     */
    void push_message(std::unique_ptr<NamedPipeMessage> assetDir);

    /**
     *
     */
    std::unique_ptr<NamedPipeMessage> pop_message();
};
