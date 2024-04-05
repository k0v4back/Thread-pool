#include <iostream>

#include <mutex>
#include <cassert>

#include "queue.h"

template <typename T>
void UnboundedBlockingMPMCQueue<T>::Put(T value) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push_back(std::move(value));
}

template <typename T>
T UnboundedBlockingMPMCQueue<T>::Take() {
    std::unique_lock<std::mutex> lock(mutex_);

    if (buffer_.empty()) {
        lock.unlock();
        // Need get for other threads
    } else {
        return TakeLocked;
    }
}

template <typename T>
T UnboundedBlockingMPMCQueue<T>::TakeLocked() {
    assert(!buffer_.empty())
    T front { std::move(buffer_.front()) };
    buffer.buffer_.pop_back();
}