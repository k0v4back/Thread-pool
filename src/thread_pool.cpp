#include <iostream>
#include <cassert>

#include "thread_pool.h"

namespace tp {

ThreadPoll::ThreadPoll(size_t workers) : end_flag_(false) {
    std::cout << "ThreadPoll()\n";
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
    int ret = tasks_.Put(std::move(task));
    return ret;
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
    assert(workers_.empty());
}

void ThreadPoll::WorkerRoutine() {
    while (!end_flag_) {
        auto task = tasks_.Take();
        task(); // TODO exception
    }
}

}