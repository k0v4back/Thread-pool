#include <iostream>
#include <cassert>

#include "thread_pool.h"

namespace tp {

ThreadPoll::ThreadPoll(size_t workers) {

    if (workers == 0) {
        throw std::invalid_argument(
            "Thread pool must have at least one worker"
        );
    }

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
    Stop();

    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

size_t ThreadPoll::Submit(Task task) {
    std::lock_guard<std::mutex> lock(thread_pool_mutex_);

    if (!task) {
        throw std::invalid_argument("Task must not be empty");
    }

    if (end_flag_) {
        throw std::runtime_error("Thread pool is stopped");
    }

    return tasks_.Put(std::move(task));
}

bool ThreadPoll::TaskComplete(size_t task_id) {
    return tasks_.TaskCompleteQueue(task_id);
}

void ThreadPoll::WaitAll() {
    tasks_.WaitAllQueue();
}

void ThreadPoll::Wait(size_t task_id) {
    tasks_.WaitQueue(task_id);
}

void ThreadPoll::Stop() {
    std::lock_guard<std::mutex> lock(thread_pool_mutex_);

    if (end_flag_) {
        return;
    }

    end_flag_ = true; //Set stop flag for ThreadPoll
    tasks_.EndWork();
}

void ThreadPoll::WorkerRoutine() {
    while (true) {
        auto item = tasks_.Take();
        if (!item) {
            break;
        }

        auto& task = item->first;
        auto task_id = item->second;

        try {
            task();
        } catch (...) {
            throw; // TODO: Fix it
        }

        tasks_.MarkCompleted(task_id);
    }
}

}