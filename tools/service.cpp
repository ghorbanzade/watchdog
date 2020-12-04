/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "watchdog/watchdog.hpp"
#include <iostream>

/**
 *
 */
int main()
{
    std::cout << "hello " << watchdog::get_watchdog() << std::endl;
}
