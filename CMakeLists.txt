### PROJECT
cmake_minimum_required(VERSION 3.19.6)
project(TiledCpp)

## OPTIONS

option(BUILD_TESTS "Build unit test executable (GoogleTest)" OFF)

### REQUIREMENTS

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

### LIBRARY

add_library(${CMAKE_PROJECT_NAME})

target_include_directories(${CMAKE_PROJECT_NAME} 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        "third-party"
)

target_sources(${CMAKE_PROJECT_NAME}
    PUBLIC 
        "tiledcpp/config.hpp"    
        "tiledcpp/tiledcpp.hpp"
        "tiledcpp/detail/parse_helpers.hpp"

    PRIVATE
        "tiledcpp/tiledcpp.cpp"
        "tiledcpp/detail/parse_helpers.cpp"
)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TILEDCPP_EXPORT)
endif()

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall PRIVATE -Wextra)

### TESTS

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tiledcpp_tests)
endif()
