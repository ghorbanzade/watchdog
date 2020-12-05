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

/**
 *
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