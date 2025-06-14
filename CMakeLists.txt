### PROJECT
cmake_minimum_required(VERSION 3.19.6)
project(TiledCpp)

## OPTIONS

option(BUILD_TESTS "Build unit test executable (GoogleTest)" OFF)

### REQUIREMENTS

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### LIBRARY

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        "third-party"
)

target_sources(${PROJECT_NAME}
    PUBLIC 
        "tiledcpp/config.hpp"    
        "tiledcpp/tiledcpp.hpp"
        "tiledcpp/types/result.hpp"
        "tiledcpp/types/math.hpp"
        "tiledcpp/types/image.hpp" 
        "tiledcpp/detail/parse_helpers.hpp"

    PRIVATE
        "tiledcpp/tiledcpp.cpp"
        "tiledcpp/types/image.cpp" 
        "tiledcpp/types/math.cpp" 
        "tiledcpp/types/properties.cpp" 
        "tiledcpp/detail/parse_helpers.cpp"
)

if (BUILD_SHARED_LIBS)
    message(STATUS "Building TiledCPP as shared lib")
    target_compile_definitions(${PROJECT_NAME} PRIVATE TILEDCPP_EXPORT)
else()
    message(STATUS "Building TiledCPP as static lib")
    target_compile_definitions(${PROJECT_NAME} PUBLIC TILEDCPP_STATIC)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -Wall PRIVATE -Wextra)

### TESTS

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tiledcpp_tests)
endif()
