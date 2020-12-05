#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

include(GNUInstallDirs)

watchdog_find_package("spdlog")

add_executable(watchdog-daemon "")

set_target_properties(
        watchdog-daemon
    PROPERTIES
        OUTPUT_NAME watchdogd
)

target_sources(
        watchdog-daemon
    PRIVATE
        service.cpp
        named_pipe_reader.cpp
        named_pipe_reader.hpp
)

target_link_libraries(
        watchdog-daemon
    PRIVATE
        watchdog
        spdlog::spdlog
)

source_group(
    TREE ${CMAKE_CURRENT_LIST_DIR}
    FILES $<TARGET_PROPERTY:watchdog-daemon,SOURCES>
)

install(TARGETS watchdog-daemon)
