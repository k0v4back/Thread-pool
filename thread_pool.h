#pragma once

#include <iostream>
#include <thread>
#include <vector>

#include "task.h"

class ThreadPoll {
private:
    std::vector<std::thread> workers_;
public:
    explicit ThreadPoll(size_t workers);
    ~ThreadPoll();

    ThreadPoll(const ThreadPoll&) = delete;
    ThreadPoll& operator=(const ThreadPoll&) = delete;

    void Submit(Task task);
    void Wait();
    void Stop();
};