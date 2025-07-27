cmake_minimum_required(VERSION 3.22)

project(ThreadPool)

# Specifies debug and release builds.
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0") # Flags for Debug: enable debug information, no optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-O3")  # Flags for Release: Maximum Optimization

# Make sure the CMAKE_BUILD_TYPE variable is set (defaults to Debug if not specified)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build: Debug or Release" FORCE)
endif()

set(SOURCES
    src/thread_pool.cpp
    tests/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)