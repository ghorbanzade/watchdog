/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

#include "watchdog/lib_api.hpp"
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <optional>
#include <queue>

struct WATCHDOG_API NamedPipeMessage
{
    const std::string _mode;
    const std::optional<std::string> _filepath;
    static std::unique_ptr<NamedPipeMessage> deserialize(const std::string& message);

    NamedPipeMessage(const std::string& mode, const std::string& filepath)
        : _mode(mode)
        , _filepath(filepath.empty() ? std::optional<std::string>() : filepath)
    {
    }
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