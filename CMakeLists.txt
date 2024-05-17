cmake_minimum_required(VERSION 3.22)

project(ThreadPool)

set(SOURCES
        src/thread_pool.cpp
        # tests/main.cpp
)

# Address sanitizer
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)
# add_link_options(-fsanitize=address -static-libasan)

# add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME}
        STATIC
                ${SOURCES}
)

set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)