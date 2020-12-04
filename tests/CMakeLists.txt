#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

watchdog_find_package("Catch2")

add_executable(watchdog_tests "")

target_sources(
        watchdog_tests
    PRIVATE
        watchdog.cpp
        main.cpp
)

target_link_libraries(
        watchdog_tests
    PRIVATE
        watchdog
        Catch2::Catch2
)

set_target_properties(
        watchdog_tests
    PROPERTIES
        DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
        RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}"
)

source_group(
    TREE
        ${CMAKE_CURRENT_LIST_DIR}
    FILES
        $<TARGET_PROPERTY:watchdog_tests,SOURCES>
)

enable_testing()

add_test(
    NAME watchdog_tests
    COMMAND $<TARGET_FILE:watchdog_tests>
)
