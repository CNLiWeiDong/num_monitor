cmake_minimum_required(VERSION 3.5)
project(qdii_monitor_plugin)

file(GLOB HEADERS "include/hb/qdii_monitor_plugin/*.h"
        )

add_library(qdii_monitor_plugin
        qdii_monitor_plugin.cpp
        qdii_monitor_plugin_impl.cpp
        monitor_targets.cpp
        monitor_singles.cpp
        monitor_pairs.cpp
        ${HEADERS}
        )

target_include_directories( qdii_monitor_plugin
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include"
        )

target_link_libraries( qdii_monitor_plugin
        appbase
        hb
        log_plugin
        dingtalk_plugin
        thread_pool_plugin
        )