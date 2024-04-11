#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <condition_variable>

template <typename T>
class UnboundedBlockingMPMCQueue {
public:
    void Put(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.push_back(std::move(value));

        //Put one element in queue and woke up one thread,
        //which may have been waiting for an element
        not_empty_cv_.notify_one();
    }

    T Take() {
        std::unique_lock<std::mutex> lock(mutex_);

        while (buffer_.empty()) {
            //Unlock mutex and wait until the queue is not empty
            not_empty_cv_.wait(lock);
        }

        return TakeLocked();
    }

private:
    //Retrieves an item from the beginning of the queue
    //assuming that the queue is not empty
    T TakeLocked() {
        assert(!buffer_.empty());
        T front = std::move(buffer_.front());
        buffer_.pop_front();
        return front;
    }

private:
    std::deque<T> buffer_;
    std::mutex mutex_;
    std::condition_variable not_empty_cv_;
};