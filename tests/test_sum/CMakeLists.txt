add_executable(test_sum
test_sum.cpp
)

#Link test_sum with GTest and ThreadPool_lib
target_link_libraries(test_sum
    PRIVATE
        GTest::gtest_main
        ThreadPool_lib
)

target_include_directories(test_sum
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(test_sum)