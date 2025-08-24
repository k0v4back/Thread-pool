#include <iostream>
#include <thread>
#include <vector>
#include <gtest/gtest.h>

#include "thread_pool.h"

using namespace std::chrono_literals;

void sum(std::vector<int>& arr, int& ans) {
    for (size_t i = 0; i < arr.size(); ++i) {
        ans += arr[i];
    }
}

TEST(SharedCounter, OneThread)
{
    //Create pool of 3 thread
    tp::ThreadPoll pool {3};

    std::vector<int> s1 = {1, 2, 3};
    int ans1 = 0;
    
    std::vector<int> s2 = {4, 5};
    int ans2 = 0;
    
    std::vector<int> s3 = {8, 9, 10};
    int ans3 = 0;

    size_t task_id1 = pool.Submit([&]() { sum(s1, ans1); });
    size_t task_id2 = pool.Submit([&]() { sum(s2, ans2); });
    size_t task_id3 = pool.Submit([&]() { sum(s3, ans3); });

    if (pool.TaskComplete(task_id1)) {
        std::cout << "task_id1 = " << task_id1 << " result = " << ans1 << std::endl;
    } else {
        pool.Wait(task_id1);
        std::cout << "task_id1 = " << task_id1 << " result = " << ans1 << std::endl;
    }

    //Wait until thread pool becomes empty
    pool.WaitAll();

    std::cout << "task_id1 = " << task_id1 << " result = " << ans1 << std::endl;
    std::cout << "task_id2 = " << task_id2 << " result = " << ans2 << std::endl;
    std::cout << "task_id3 = " << task_id3 << " result = " << ans3 << std::endl;

    EXPECT_EQ(ans1, 6);
    EXPECT_EQ(ans2, 9);
    EXPECT_EQ(ans3, 27);


    // Stop all threads
    pool.Stop(); 
}