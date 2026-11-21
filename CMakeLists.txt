cmake_minimum_required(VERSION 4.4)
project(all_app)

set(CMAKE_CXX_STANDARD 20)

add_executable(all_app
        src/main.cpp
        src/ByteParser.cpp
        include/ByteParser.h)

target_include_directories(all_app PRIVATE include)
