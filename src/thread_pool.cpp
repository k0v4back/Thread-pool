#include <iostream>
#include <cassert>

#include "thread_pool.h"

namespace tp {

ThreadPoll::ThreadPoll(size_t workers) : end_flag_(false) {
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
        throw;
    }
}

ThreadPoll::~ThreadPoll() {
    assert(workers_.empty());
}

// template <typename Func, typename ...Args>
// size_t ThreadPoll::Submit(const Func& value, Args&&... args) {
//     int ret = tasks_.Put(value, args...);
//     return ret;
// }

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
    end_flag_ = true;
    for (auto& worker: workers_) {
        tasks_.Abort(end_flag_);
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPoll::WorkerRoutine() {
    while (!end_flag_) {
        // auto task = tasks_.Take(end_flag_);
        // task(); // TODO exception
        tasks_.Take(end_flag_);
    }
}

}