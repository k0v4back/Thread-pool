#include <iostream>
#include <vector>

#include "../src/thread_pool.h"

#include <thread>
using namespace std::chrono_literals;

void sum(std::vector<int>& arr, int& ans) {
    for (size_t i = 0; i < arr.size(); ++i) {
        ans += arr[i];
    }
}

// void testSum() {
//     //Create pool of 3 thread
//     tp::ThreadPoll pool {3};

//     std::vector<int> s1 = {1, 2, 3};
//     int ans1 = 0;
    
//     std::vector<int> s2 = {4, 5};
//     int ans2 = 0;
    
//     std::vector<int> s3 = {8, 9, 10};
//     int ans3 = 0;

//     // size_t task_id1 = pool.Submit([&]() { sum(s1, ans1); });
//     // size_t task_id2 = pool.Submit([&]() { sum(s2, ans2); });
//     // size_t task_id3 = pool.Submit([&]() { sum(s3, ans3); });

//     auto task_id1 = pool.Submit(sum, std::ref(ans1), std::ref(s1));
//     auto task_id2 = pool.Submit(sum, std::ref(ans2), std::ref(s2));
//     auto task_id3 = pool.Submit(sum, std::ref(ans3), std::ref(s3));

//     if (pool.TaskComplete(task_id1)) {
//         std::cout << "task_id1 = " << task_id1 << " result = " << ans1 << std::endl;
//     } else {
//         pool.Wait(task_id1);
//         std::cout << "task_id1 = " << task_id1 << " result = " << ans1 << std::endl;
//     }

//     //Wait until thread pool becomes empty
//     pool.WaitAll();

//     std::cout << "task_id2 = " << task_id2 << " result = " << ans2 << std::endl;
//     std::cout << "task_id3 = " << task_id3 << " result = " << ans3 << std::endl;

//     //Stop all threads
//     pool.Stop();
// }

// void testSharedCounter() {
//     //Create pool of 3 thread
//     tp::ThreadPoll pool {3};

//     size_t shared_counter = 0;

//     for (size_t i = 0; i < 100500; i++) {
//         pool.Submit([&]() {
//             //Some payload
//             shared_counter++;
//         });
//     }

//     //Wait until thread pool becomes empty
//     pool.WaitAll();

//     std::cout << "shared_counter = " << shared_counter << std::endl;
//     std::this_thread::sleep_for(1s);
//     std::cout << "shared_counter = " << shared_counter << std::endl;

//     //Stop all threads
//     pool.Stop();
// }


int int_sum(int a, int b) {
    return a + b;
}

void void_sum(int& c, int a, int b) {
    c = a + b;
}

void void_without_argument() {
    std::cout << "It's OK!" << std::endl;
}

int main() {
    // std::cout << "----------TEST1----------\n";
    // testSum();

    // std::cout << "\n\n\n----------TEST2----------\n";
    // testSharedCounter();

    tp::ThreadPoll pool {3};
    int c;

    pool.Submit(int_sum, 2, 3);               // id = 0
    pool.Submit(void_sum, std::ref(c), 4, 6); // id = 1
    pool.Submit(void_without_argument);       // id = 2

    pool.Wait(0);
    std::cout << c << std::endl;

    pool.WaitAll();

    return 0;
}