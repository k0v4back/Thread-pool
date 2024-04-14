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
    end_flag_ = true;
    for (size_t i = 0; i < workers_.size(); i++) {
        tasks_.GetNotEmptyCv().notify_all();
        workers_[i].join();
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
    tasks_.WaitAllQueue(lock);
}

void ThreadPoll::Wait(size_t task_id) {
    std::unique_lock<std::mutex> lock(thread_pool_mutex_);
    tasks_.WaitQueue(task_id, lock);
}

void ThreadPoll::Stop() {
    assert(workers_.empty());
}

void ThreadPoll::WorkerRoutine() {
    while (!end_flag_) {
        auto task = tasks_.Take();
        task(); // TODO exception
    }
}