#include <iostream>
#include <thread>
#include <gtest/gtest.h>

#include "thread_pool.h"

using namespace std::chrono_literals;

TEST(SharedCounter, OneThread)
{
    //Create pool of 1 thread
    tp::ThreadPoll pool {1};

    size_t shared_counter = 0;
    size_t num = 100500;

    for (size_t i = 0; i < num; i++) {
        pool.Submit([&]() {
            //Some payload
            shared_counter++;
        });
    }

    //Wait until thread pool becomes empty
    pool.WaitAll();

    std::cout << "shared_counter = " << shared_counter << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "shared_counter = " << shared_counter << std::endl;

    EXPECT_EQ(num, shared_counter);

    //Stop all threads
    pool.Stop(); 
}

TEST(SharedCounter, TenThreads)
{
    //Create pool of 10 threads
    tp::ThreadPoll pool {10};

    size_t shared_counter = 0;
    size_t num = 100500;

    for (size_t i = 0; i < num; i++) {
        pool.Submit([&]() {
            //Some payload
            shared_counter++;
        });
    }

    //Wait until thread pool becomes empty
    pool.WaitAll();

    std::cout << "shared_counter = " << shared_counter << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "shared_counter = " << shared_counter << std::endl;

    EXPECT_NE(num, shared_counter);

    //Stop all threads
    pool.Stop(); 
}

TEST(SharedCounter, OneHundredThreads)
{
    //Create pool of 100 threads
    tp::ThreadPoll pool {100};

    size_t shared_counter = 0;
    size_t num = 100500;

    for (size_t i = 0; i < num; i++) {
        pool.Submit([&]() {
            //Some payload
            shared_counter++;
        });
    }

    //Wait until thread pool becomes empty
    pool.WaitAll();

    std::cout << "shared_counter = " << shared_counter << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "shared_counter = " << shared_counter << std::endl;

    EXPECT_NE(num, shared_counter);

    //Stop all threads
    pool.Stop(); 
}