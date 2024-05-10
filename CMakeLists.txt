cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME
        ThreadPool
)

project(${PROJECT_NAME})

set(SOURCES
        src/thread_pool.cpp
        # tests/main.cpp
)

set(HEADERS
        src/thread_pool.h
        src/task.h
        src/queue.h
)

# add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)