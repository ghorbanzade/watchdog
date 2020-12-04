/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#pragma once

/**
 * @file watchdog.hpp
 *
 * @brief Entry-point to the watchdog library.
 *
 * @author Pejman Ghorbanzade <pejman@ghorbanzade.com>
 * @date 2020
 */

#include "watchdog/lib_api.hpp"

/**
 * @namespace watchdog
 *
 * @brief Provides the interface to the watchdog library.
 */
namespace watchdog {

    /**
     * Returns 42.
     * 
     * @return Returns 42.
     */
    int WATCHDOG_API get_watchdog();

} // namespace watchdog
