#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <unordered_set>

#include "task.h"
#include "queue.h"

class ThreadPoll {
public:
    explicit ThreadPoll(size_t workers);
    ~ThreadPoll();

    ThreadPoll(const ThreadPoll&) = delete;
    ThreadPoll& operator=(const ThreadPoll&) = delete;

    size_t Submit(Task task);
    bool TaskComplete(size_t task_id);
    void WaitAll();
    void Wait(size_t task_id);
    void Stop();

private:
    void WorkerRoutine();   //Procedure for each execution thread

private:
    std::vector<std::thread> workers_;
    UnboundedBlockingMPMCQueue<Task> tasks_;
    std::atomic<bool> end_flag_ { false }; //Flag end of thread pool work
    std::mutex completed_task_ids_mutex_; 
    std::mutex thread_pool_mutex_;
};