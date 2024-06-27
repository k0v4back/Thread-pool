#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <unordered_set>

#include "task.h"
#include "queue.h"

namespace tp {

class ThreadPoll {
public:
    explicit ThreadPoll(size_t workers);
    ~ThreadPoll();

    ThreadPoll(const ThreadPoll&) = delete;
    ThreadPoll& operator=(const ThreadPoll&) = delete;

    template <typename Func, typename ...Args, typename ...FuncTypes>
    size_t Submit(Func(*func)(FuncTypes...), Args&&... args) {
        int ret = tasks_.Put(func, args...);
        return ret;
    }

    bool TaskComplete(size_t task_id);
    void WaitAll();
    void Wait(size_t task_id);
    void Stop();

private:
    void WorkerRoutine();   /* Procedure for each execution thread */

private:
    std::vector<std::thread> workers_;          /* Array of threads */
    UnboundedBlockingMPMCQueue<Task> tasks_;    /* Queue of tasks */
    std::atomic<bool> end_flag_;                /* Flag end of thread pool work */
};

}