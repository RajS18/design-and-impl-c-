cmake_minimum_required(VERSION 3.10)
project(rc_gc_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rc_gc
    src/main.cpp
)

target_include_directories(rc_gc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
