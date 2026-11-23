cmake_minimum_required(VERSION 4.4)
project(all_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(all_app
        src/main.cpp
        src/ByteParser.cpp
        include/ByteParser.h
        include/HelperReaders.h)

target_include_directories(all_app PRIVATE include)

include(CTest)
if(BUILD_TESTING)
    find_package(GTest REQUIRED)

    add_executable(HelperReadersTests
            tests/HelperReadersTests.cpp)
    target_include_directories(HelperReadersTests PRIVATE include)
    target_link_libraries(HelperReadersTests PRIVATE GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(HelperReadersTests)
endif()
