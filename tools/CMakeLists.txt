#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

include(GNUInstallDirs)

add_executable(watchdog_service "")

target_sources(
        watchdog_service
    PRIVATE
        service.cpp
)

target_link_libraries(
        watchdog_service
    PRIVATE
        watchdog
)

source_group(
    TREE ${CMAKE_CURRENT_LIST_DIR}
    FILES $<TARGET_PROPERTY:watchdog_service,SOURCES>
)

install(TARGETS watchdog_service)
