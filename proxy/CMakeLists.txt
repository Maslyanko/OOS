cmake_minimum_required(VERSION 3.10)

project(ProxyServer VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    src/main.cpp
    src/proxy/proxy_task.cpp
    src/proxy/proxy.cpp
    src/proxy/request_monitor.cpp
    src/util/error/error.cpp
    src/util/http_parser/http_parser.cpp
    src/multithreading/thread_pool.cpp
    src/cache/cache.cpp
)

add_executable(proxy_server ${SOURCES})

target_include_directories(proxy_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/proxy
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/error
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/http_parser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/multithreading
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cache
)

target_compile_options(proxy_server PRIVATE -Wall -Wextra -pedantic)