#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

watchdog_find_package("Catch2")

add_executable(watchdog-tests "")

target_sources(
        watchdog-tests
    PRIVATE
        watchdog.cpp
        main.cpp
)

target_link_libraries(
        watchdog-tests
    PRIVATE
        watchdog
        Catch2::Catch2
)

set_target_properties(
        watchdog-tests
    PROPERTIES
        DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
        RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}"
)

source_group(
    TREE
        ${CMAKE_CURRENT_LIST_DIR}
    FILES
        $<TARGET_PROPERTY:watchdog-tests,SOURCES>
)

enable_testing()

add_test(
    NAME watchdog-tests
    COMMAND $<TARGET_FILE:watchdog-tests>
)
