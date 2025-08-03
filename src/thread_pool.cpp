#include <iostream>
#include <cassert>

#include "thread_pool.h"

namespace tp {

ThreadPoll::ThreadPoll(size_t workers) {
    try {
        //Start worker threads
        workers_.reserve(workers);
        for (size_t i = 0; i < workers; i++) {
            workers_.emplace_back([this](){
                WorkerRoutine();
            });
        }
    } catch (...) {
        tasks_.EndWork();
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        throw;
    }
}

ThreadPoll::~ThreadPoll() {
    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

size_t ThreadPoll::Submit(Task task) {
    return tasks_.Put(std::move(task));
}

bool ThreadPoll::TaskComplete(size_t task_id) {
    std::lock_guard<std::mutex> lock(completed_task_ids_mutex_);
    return tasks_.TaskCompleteQueue(task_id);
}

void ThreadPoll::WaitAll() {
    std::unique_lock<std::mutex> lock(thread_pool_mutex_);
    tasks_.WaitAllQueue();
}

void ThreadPoll::Wait(size_t task_id) {
    std::unique_lock<std::mutex> lock(thread_pool_mutex_);
    tasks_.WaitQueue(task_id);
}

void ThreadPoll::Stop() {
    std::unique_lock<std::mutex> lock(thread_pool_mutex_);
    tasks_.EndWork();
}

void ThreadPoll::WorkerRoutine() {
    while (true) {
        auto task = tasks_.Take();
        if (!task) {
            break;
        }
        (*task)();
    }
}

}