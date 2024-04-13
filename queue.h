#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <unordered_set>

template <typename T>
class UnboundedBlockingMPMCQueue {
public:
    void Put(T value) {
        std::lock_guard<std::mutex> lock(queue_mutex_);

        size_t task_id = last_idx_++;
        buffer_.emplace(std::move(value), task_id);

        //Put one element in queue and woke up one thread,
        //which may have been waiting for an element
        not_empty_cv_.notify_one();
    }

    T Take() {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        while (buffer_.empty()) {
            //Unlock mutex and wait until the queue is not empty
            not_empty_cv_.wait(lock);
        }

        return TakeLocked();
    }

    std::condition_variable& GetNotEmptyCv() {
        return not_empty_cv_;
    }

    std::condition_variable& GetCompletedTaskIDsCv() {
        return completed_task_ids_cv_;
    }

    std::mutex& GetQueueMutex() {
        return queue_mutex_;
    }

    std::deque<std::pair<T, size_t>>& GetQueueBuffer() const {
        return buffer_;
    }

    std::atomic<size_t>& GetLastIdx() const {
        return buffer_;
    }

private:
    //Retrieves an item from the beginning of the queue
    //assuming that the queue is not empty
    T TakeLocked() {
        assert(!buffer_.empty());
        T front = std::move(buffer_.front());
        buffer_.pop_front();

        completed_task_ids_.insert(front.second);
        completed_task_ids_cv_.notify_all();

        return front;
    }

private:
    std::deque<std::pair<T, size_t>> buffer_; //Task and number of task
    std::mutex queue_mutex_;
    std::condition_variable not_empty_cv_; //Not empty queue cv
    std::condition_variable completed_task_ids_cv_;
    std::unordered_set<size_t> completed_task_ids_; 
    std::atomic<size_t> last_idx_ {0};
};