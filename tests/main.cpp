#include <iostream>
#include <thread>

#include "../thread_pool.h"

int main() {
    //Create pool of 4 thread
    ThreadPoll pool {4};

    size_t shared_counter = 0;

    for (size_t i = 0; i < 100500; i++) {
        pool.Submit([&]() {
            shared_counter++;
        });
    }

    pool.Wait();

    return 0;
}