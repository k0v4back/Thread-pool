#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <condition_variable>

template <typename T>
class UnboundedBlockingMPMCQueue {
public:
    void Put(T value);
    T Take();

private:
    T TakeLocked(); //Retrieves an item from the head of the queue

private:
    std::deque<T> buffer_;
    std::mutex mutex_;
    std::condition_variable not_empty_cv_;
};