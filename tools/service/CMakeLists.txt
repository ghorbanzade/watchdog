#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

include(GNUInstallDirs)

watchdog_find_package("spdlog")

add_executable(watchdog-service "")

set_target_properties(
        watchdog-service
    PROPERTIES
        OUTPUT_NAME watchdogd
)

target_sources(
        watchdog-service
    PRIVATE
        service.cpp
        named_pipe_reader.cpp
        named_pipe_reader.hpp
)

target_link_libraries(
        watchdog-service
    PRIVATE
        watchdog
        spdlog::spdlog
)

source_group(
    TREE ${CMAKE_CURRENT_LIST_DIR}
    FILES $<TARGET_PROPERTY:watchdog-service,SOURCES>
)

install(TARGETS watchdog-service)
