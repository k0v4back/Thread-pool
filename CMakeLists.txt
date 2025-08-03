cmake_minimum_required(VERSION 3.22)
project(ThreadPool VERSION 0.1.0)

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
enable_testing()

add_executable(test_ThreadPool
    tests/main.cpp
)
target_link_libraries(test_ThreadPool PRIVATE ThreadPool_lib) # Use library ThreadPool_lib in test_ThreadPool
add_test(NAME ThreadPool_test COMMAND test_ThreadPool)