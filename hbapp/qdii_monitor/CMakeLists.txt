cmake_minimum_required(VERSION 3.5)
project(qdii_monitor)

set(CMAKE_CXX_STANDARD 14)


add_executable(qdii_monitor main.cpp)

target_include_directories(qdii_monitor
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
        )

target_link_libraries(qdii_monitor
        PRIVATE appbase
        PRIVATE ${Boost_LIBRARIES} #boost test
        PRIVATE -Wl,${whole_archive_flag} log_plugin                            -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} qdii_monitor_plugin                   -Wl,${no_whole_archive_flag}
        # PRIVATE -Wl,${whole_archive_flag} send_mail_plugin                      -Wl,${no_whole_archive_flag}
        )