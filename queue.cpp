#include <iostream>

#include <mutex>
#include <cassert>
#include <condition_variable>

#include <thread>

#include "queue.h"

template <typename T>
void UnboundedBlockingMPMCQueue<T>::Put(T value) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push_back(std::move(value));

    //Put one element in queue and woke up one thread,
    //which may have been waiting for an element
    not_empty_cv_.notify_one();
}

template <typename T>
T UnboundedBlockingMPMCQueue<T>::Take() {
    std::unique_lock<std::mutex> lock(mutex_);

    while (buffer_.empty()) {
        //Unlock mutex and wait until the queue is not empty
        not_empty_cv_.wait(lock);
    }

    return TakeLocked();
}

template <typename T>
T UnboundedBlockingMPMCQueue<T>::TakeLocked() {
    assert(!buffer_.empty());
    T front { std::move(buffer_.front()) };
    buffer_.pop_back();
    return front;
}