#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

include(GNUInstallDirs)

watchdog_find_package("cxxopts")

add_executable(watchdog-bin "")

set_target_properties(
        watchdog-bin
    PROPERTIES
        OUTPUT_NAME watchdog
)

target_sources(
        watchdog-bin
    PRIVATE
        frontend.cpp
)

target_link_libraries(
        watchdog-bin
    PRIVATE
        watchdog
        cxxopts::cxxopts
)

source_group(
    TREE ${CMAKE_CURRENT_LIST_DIR}
    FILES $<TARGET_PROPERTY:watchdog-bin,SOURCES>
)

install(TARGETS watchdog-bin)
