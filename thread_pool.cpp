#include <iostream>
#include <cassert>

#include "thread_pool.h"

ThreadPoll::ThreadPoll(size_t workers) {
    //Start worker threads
    workers_.reserve(workers);
    for (size_t i = 0; i < workers; i++) {
        workers_.emplace_back([this](){
            WorkerRoutine();
        });
    }
}

ThreadPoll::~ThreadPoll() {
    for (size_t i = 0; i < workers_.size(); i++) {
        tasks_.GetNotEmptyCv().notify_all();
        workers_[i].join();
    }
}

void ThreadPoll::Submit(Task task) {
    tasks_.Put(std::move(task));
}

void ThreadPoll::WaitAll() {
    std::unique_lock<std::mutex> lock(thread_pool_mutex_);

    tasks_.GetCompletedTaskIDsCv().wait(lock, [this]()->bool {
        std::lock_guard<std::mutex> task_lock(tasks_.GetQueueMutex());
        return tasks_.GetQueueBuffer().empty() && tasks_.GetLastIdx() == completed_task_ids_.size();
    });
}

void ThreadPoll::Wait(size_t task_id) {
    std::unique_lock<std::mutex> lock(thread_pool_mutex_);

    tasks_.GetCompletedTaskIDsCv().wait(lock, [this, task_id]()->bool {
        return completed_task_ids_.find(task_id) != completed_task_ids_.end(); 
    });
}

void ThreadPoll::Stop() {
    assert(workers_.empty());
}

void ThreadPoll::WorkerRoutine() {
    while (true) {
        auto task = tasks_.Take();
        task(); // TODO exception
    }
}