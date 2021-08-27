cmake_minimum_required(VERSION 3.5)
project(num_monitor)

set(CMAKE_CXX_STANDARD 14)


add_executable(num_monitor monitor.cpp)

target_include_directories(num_monitor
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
        )

target_link_libraries(num_monitor
        PRIVATE appbase
        PRIVATE ${Boost_LIBRARIES} #boost test
        PRIVATE -Wl,${whole_archive_flag} log_plugin                            -Wl,${no_whole_archive_flag}
        PRIVATE -Wl,${whole_archive_flag} monitor_price_plugin                  -Wl,${no_whole_archive_flag}
        # PRIVATE -Wl,${whole_archive_flag} send_mail_plugin                      -Wl,${no_whole_archive_flag}
        )