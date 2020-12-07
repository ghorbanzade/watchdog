#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

include(GNUInstallDirs)

add_executable(watchdog-daemon "")

set_target_properties(
        watchdog-daemon
    PROPERTIES
        OUTPUT_NAME watchdogd
)

target_sources(
        watchdog-daemon
    PRIVATE
        main.cpp
)

target_link_libraries(
        watchdog-daemon
    PRIVATE
        watchdog
)

source_group(
    TREE ${CMAKE_CURRENT_LIST_DIR}
    FILES $<TARGET_PROPERTY:watchdog-daemon,SOURCES>
)

install(TARGETS watchdog-daemon
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT Watchdog_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Watchdog_Runtime
            NAMELINK_COMPONENT Watchdog_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Watchdog_Development
)
