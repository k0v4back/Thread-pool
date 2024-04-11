#include <iostream>
#include <cassert>

#include "thread_pool.h"

#include <thread>
using namespace std::chrono_literals;

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
    assert(workers_.empty());
}

void ThreadPoll::Submit(Task task) {
    tasks_.Put(std::move(task));
}

void ThreadPoll::Wait() {
    std::this_thread::sleep_for(3s); //need to change
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