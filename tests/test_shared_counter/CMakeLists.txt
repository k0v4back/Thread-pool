add_executable(test_shared_counter
    test_shared_counter.cpp
)

#Link test_shared_counter with GTest and ThreadPool_lib
target_link_libraries(test_shared_counter
    PRIVATE
        GTest::gtest_main
        ThreadPool_lib
)

target_include_directories(test_shared_counter
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(test_shared_counter)