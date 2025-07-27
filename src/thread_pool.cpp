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
        end_flag_ = true;
        tasks_.NotifyAllWorkers();
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        throw;
    }
}

ThreadPoll::~ThreadPoll() {
    std::cout << "~ThreadPoll()" << std::endl;
    end_flag_ = true;
    tasks_.NotifyAllWorkers();
    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

size_t ThreadPoll::Submit(Task task) {
    try {
        return tasks_.Put(std::move(task), end_flag_);
    } catch (...) {
        end_flag_ = true;
        // tasks_.NotifyAllWorkers();
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
}

bool ThreadPoll::TaskComplete(size_t task_id) {
    // std::lock_guard<std::mutex> lock(completed_task_ids_mutex_);
    return tasks_.TaskCompleteQueue(task_id);
}

void ThreadPoll::WaitAll() {
    // std::unique_lock<std::mutex> lock(thread_pool_mutex_);
    tasks_.WaitAllQueue();
}

void ThreadPoll::Wait(size_t task_id) {
    // std::unique_lock<std::mutex> lock(thread_pool_mutex_);
    tasks_.WaitQueue(task_id);
}

void ThreadPoll::Stop() {
    std::cout << "Stop" << std::endl;
    {
        std::unique_lock<std::mutex> lock(thread_pool_mutex_);
        end_flag_ = true;
    }
    tasks_.NotifyAllWorkers();
}

void ThreadPoll::WorkerRoutine() {
    while (true) {
        std::cout << "WorkerRoutine " << std::endl;
        if (end_flag_) {
            std::cout << "Thread " << std::this_thread::get_id() << " exiting\n";
            return;
        }

        try {
            auto task = tasks_.Take(end_flag_);
            task(); // TODO exception
        } catch (...) {
            end_flag_ = true;
            // tasks_.NotifyAllWorkers();
            for (std::thread& worker : workers_) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }
    }
}

}