#pragma once

#include <iostream>
#include <thread>
#include <vector>

#include "task.h"
#include "queue.h"

class ThreadPoll {
public:
    explicit ThreadPoll(size_t workers);
    ~ThreadPoll();

    ThreadPoll(const ThreadPoll&) = delete;
    ThreadPoll& operator=(const ThreadPoll&) = delete;

    void Submit(Task task);
    void Wait();
    void Stop();

private:
    void WorkerRoutine();   //Procedure for each execution thread

private:
    std::vector<std::thread> workers_;
    UnboundedBlockingMPMCQueue<Task> tasks_;
};