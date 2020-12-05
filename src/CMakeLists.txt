#
# Copyright 2020 Pejman Ghorbanzade. All rights reserved.
#

include(GenerateExportHeader)
include(GNUInstallDirs)

watchdog_find_package("fmt")

add_library(watchdog "")

target_sources(
        watchdog
    PRIVATE
        watchdog.cpp
)

target_include_directories(
        watchdog
    PUBLIC
        ${WATCHDOG_INCLUDE_DIR}
        "${CMAKE_BINARY_DIR}/generated"
)

target_link_libraries(
        watchdog
    PUBLIC
        fmt::fmt
)

set_target_properties(
        watchdog
    PROPERTIES
        POSITION_INDEPENDENT_CODE OFF
        SOVERSION 1
        VERSION 1.0.0
)

target_compile_features(
        watchdog
    PUBLIC
        cxx_std_17
)

generate_export_header(
    watchdog
    EXPORT_MACRO_NAME "WATCHDOG_API"
    EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/generated/watchdog/lib_api.hpp"
)

install(TARGETS watchdog)
install(DIRECTORY "${WATCHDOG_ROOT_DIR}/include/watchdog"
        DESTINATION include MESSAGE_NEVER FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY "${CMAKE_BINARY_DIR}/generated/watchdog"
        DESTINATION include MESSAGE_NEVER FILES_MATCHING PATTERN "*.h*")
