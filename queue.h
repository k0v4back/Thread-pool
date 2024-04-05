#pragma once

#include <iostream>
#include <mutex>
#include <queue>

template <typename T>
class UnboundedBlockingMPMCQueue {
public:
    void Put(T value);
    T Take();

private:
    T TakeLocked();

private:
    std::deque<T> buffer_;
    std::mutex mutex_;
};