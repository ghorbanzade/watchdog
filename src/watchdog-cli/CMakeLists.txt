#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

include(GNUInstallDirs)

watchdog_find_package("cxxopts")

add_executable(watchdog-cli "")

set_target_properties(
        watchdog-cli
    PROPERTIES
        OUTPUT_NAME watchdog
)

target_sources(
        watchdog-cli
    PRIVATE
        main.cpp
)

target_link_libraries(
        watchdog-cli
    PRIVATE
        watchdog
        cxxopts::cxxopts
)

source_group(
    TREE ${CMAKE_CURRENT_LIST_DIR}
    FILES $<TARGET_PROPERTY:watchdog-cli,SOURCES>
)

install(TARGETS watchdog-cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT Watchdog_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Watchdog_Runtime
            NAMELINK_COMPONENT Watchdog_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Watchdog_Development
)