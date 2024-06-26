#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <unordered_set>
#include <future>

namespace tp {

template <typename T>
class UnboundedBlockingMPMCQueue {
public:
    template <typename Func, typename ...Args>
    size_t Put(const Func& task_func, Args&&... args) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        size_t task_id = last_idx_++;
        // buffer_.emplace_back(std::async(std::launch::deferred, task_func, args...), task_id);
        buffer_.emplace_back(std::async(std::launch::deferred, task_func, this, args...), task_id);

        //Put one element in queue and woke up one thread,
        //which may have been waiting for an element
        workers_cv_.notify_one();

        return task_id;
    }

    void Take(std::atomic<bool>& end_flag) {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        while (buffer_.empty()) {
            //Unlock mutex and wait until the queue is not empty and thread pool is still running

            // workers_cv_.wait(lock);

            workers_cv_.wait(lock, [this, &end_flag]()->bool {
                return !buffer_.empty() || end_flag;
            });
            // if (end_flag)
            //     return {};
        }

        // return TakeLocked();
        TakeLocked();
    }

    void WaitAllQueue() {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        completed_task_ids_cv_.wait(lock, [this]()->bool {
            std::lock_guard<std::mutex> task_lock(completed_task_ids_mutex_);
            return buffer_.empty() && last_idx_ == completed_task_ids_.size();
        });
    }

    void WaitQueue(size_t task_id) {
        std::unique_lock<std::mutex> lock(completed_task_ids_mutex_);
        completed_task_ids_cv_.wait(lock, [this, task_id]()->bool {
            return completed_task_ids_.find(task_id) != completed_task_ids_.end(); 
        });
    }

    bool TaskCompleteQueue(size_t task_id) {
        std::lock_guard<std::mutex> lock(completed_task_ids_mutex_);
        if (completed_task_ids_.find(task_id) != completed_task_ids_.end())
            return true;
        return false;
    }

    std::condition_variable& GetWorkersCv() {
        return workers_cv_;
    }

    void Abort(std::atomic<bool>& end_flag) {
        std::unique_lock<std::mutex> lock;
        end_flag = true;
        buffer_.clear();
        completed_task_ids_cv_.notify_all();
        workers_cv_.notify_all();
    }

private:
    //Retrieves an item from the beginning of the queue
    //assuming that the queue is not empty
    void TakeLocked() {
        assert(!buffer_.empty());
        // auto front = std::move(buffer_.front());
        auto front = std::move(buffer_.front());
        buffer_.pop_front();

        front.first.get();

        completed_task_ids_.insert(front.second);
        completed_task_ids_cv_.notify_all();

        // return front.first;
    }

private:
    std::deque<std::pair<T, size_t>> buffer_; //Task and number of task
    std::mutex queue_mutex_;
    std::mutex completed_task_ids_mutex_;
    std::condition_variable workers_cv_;
    std::condition_variable completed_task_ids_cv_;
    std::unordered_set<size_t> completed_task_ids_; 
    std::atomic<size_t> last_idx_ {0};
};

}