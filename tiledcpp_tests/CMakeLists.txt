### DECLARE TARGET

set(Tests ${CMAKE_PROJECT_NAME}Tests)
add_executable(${Tests})

### FETCH GOOGLETEST ###

include(FetchContent)

FetchContent_Declare(
        GoogleTest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
        GIT_SHALLOW TRUE
)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Couldn't get GoogleTest to build correctly as a shared library
set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(GoogleTest)

### SETUP

set_target_properties(${Tests} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_compile_options(${Tests} PRIVATE -Wall PRIVATE -Wextra)

target_link_libraries(${Tests}
    PRIVATE 
        ${CMAKE_PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)

target_sources(${Tests}
    PRIVATE
        parse_helpers_tests.cpp 
        tiledcpp_tests.cpp 
)