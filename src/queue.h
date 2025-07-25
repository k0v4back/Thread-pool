#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <unordered_set>

namespace tp {

template <typename T>
class UnboundedBlockingMPMCQueue {
public:
    size_t Put(T value) {
        std::lock_guard<std::mutex> lock(queue_mutex_);

        size_t task_id = last_idx_++;
        buffer_.emplace_back(std::move(value), task_id);

        //Put one element in queue and woke up one thread,
        //which may have been waiting for an element
        workers_cv_.notify_one();

        return task_id;
    }

    T Take() {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        while (buffer_.empty()) {
            //Unlock mutex and wait until the queue is not empty
            workers_cv_.wait(lock);
        }

        return TakeLocked();
    }

    void WaitAllQueue() {
        completed_task_ids_cv_.wait(wait_mutex_, [this]()->bool {
            std::lock_guard<std::mutex> task_lock(queue_mutex_);
            return buffer_.empty() && last_idx_ == completed_task_ids_.size();
        });
    }

    void WaitQueue(size_t task_id) {
        completed_task_ids_cv_.wait(wait_mutex_, [this, task_id]()->bool {
            return completed_task_ids_.find(task_id) != completed_task_ids_.end(); 
        });
    }

    bool TaskCompleteQueue(size_t task_id) {
        if (completed_task_ids_.find(task_id) != completed_task_ids_.end())
            return true;
        return false;
    }

    std::condition_variable& GetWorkersCv() {
        return workers_cv_;
    }

private:
    //Retrieves an item from the beginning of the queue
    //assuming that the queue is not empty
    T TakeLocked() {
        assert(!buffer_.empty());
        auto front = std::move(buffer_.front());
        buffer_.pop_front();

        completed_task_ids_.insert(front.second);
        completed_task_ids_cv_.notify_all();

        return front.first;
    }

private:
    std::deque<std::pair<T, size_t>> buffer_; //Task and id of task
    std::mutex queue_mutex_;
    std::unique_lock<std::mutex> wait_mutex_;
    std::condition_variable workers_cv_;
    std::condition_variable completed_task_ids_cv_;
    std::unordered_set<size_t> completed_task_ids_; 
    std::atomic<size_t> last_idx_ {0};
};

}