cmake_minimum_required(VERSION 3.22)
project(ThreadPool VERSION 0.1.0)

option(ENABLE_TESTS"Build and run tests" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Specifies debug and release builds.
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0") # Flags for Debug: enable debug information, no optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-O3")  # Flags for Release: Maximum Optimization

# Make sure the CMAKE_BUILD_TYPE variable is set (defaults to Debug if not specified)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build: Debug or Release" FORCE)
endif()


# Create library ThreadPool_lib
add_library(ThreadPool_lib STATIC
    src/thread_pool.cpp
)
target_include_directories(ThreadPool_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(ThreadPool_lib PUBLIC cxx_std_17)

# Tests
if(ENABLE_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_subdirectory(tests)
endif()