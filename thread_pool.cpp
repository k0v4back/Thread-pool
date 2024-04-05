#include <iostream>

#include "thread_pool.h"

ThreadPoll::ThreadPoll(size_t workers) {
    StartWorkerThread(workers);
}

ThreadPoll::~ThreadPoll() {

}

void ThreadPoll::Submit(Task task) {
    tasks_.Put(std::move(task));
}

void ThreadPoll::Wait() {

}

void ThreadPoll::Stop() {

}

void ThreadPoll::StartWorkerThread(size_t workers) {
    for (size_t i = 0; i < workers; i++) {
        std::thread thrd(WorkerRoutine);
        workers_.push_back(thrd);
    }
}

void ThreadPoll::WorkerRoutine() {
    while (true) {
        auto task = tasks_.Take();
        task();
    }
}