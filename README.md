# Thread-pool

A Thread Pool consists of a queue of tasks and several threads
that take tasks out of the queue and execute them in parallel.

## Compile

### Debug:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake --build . --config Debug
```

### Release:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

## Run

In build directory
```
./ThreadPool
```


## Example 1 - sum

```
#include <iostream>
#include <vector>

#include "thread_pool.h"

#include <thread>
using namespace std::chrono_literals;

void sum(std::vector<int>& arr, int& ans) {
    for (size_t i = 0; i < arr.size(); ++i) {
        ans += arr[i];
    }
}

void testSum() {
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

    std::cout << "task_id2 = " << task_id2 << " result = " << ans2 << std::endl;
    std::cout << "task_id3 = " << task_id3 << " result = " << ans3 << std::endl;
}

int main() {
    std::cout << "----------TEST1----------\n";
    testSum();

    return 0;
}
```

## Example 2 - shared counter

```
#include <iostream>
#include <vector>

#include "thread_pool.h"

#include <thread>
using namespace std::chrono_literals;

void testSharedCounter() {
    //Create pool of 3 thread
    tp::ThreadPoll pool {3};

    size_t shared_counter = 0;

    for (size_t i = 0; i < 100500; i++) {
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
}

int main() {
    std::cout << "\n\n\n----------TEST2----------\n";
    testSharedCounter();

    return 0;
}
```