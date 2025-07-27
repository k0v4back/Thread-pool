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
    size_t Put(T value, std::atomic<bool>& endflag) {
        std::lock_guard<std::mutex> lock(queue_mutex_);

        if (endflag) {
            // throw std::runtime_error("Cannot take task in stopped thread pool");
        }

        size_t task_id = last_idx_++;
        buffer_.emplace_back(std::move(value), task_id);

        //Put one element in queue and woke up one thread,
        //which may have been waiting for an element
        workers_cv_.notify_one();

        return task_id;
    }

    T Take(std::atomic<bool>& endflag) {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        while (buffer_.empty() && !endflag) {
            std::cout << "1 Take" << std::endl; // Проблема в том, что при нотификации всех потоков мы хотим чтобы они проснули
                                              // и увидели что программа должна завершиться т.е. endflag выставляется в true
                                              // но потоки просываются и снова засыпают т.к. очередь пуста
                                              // т.е. нужно тупо выйти отсюда какием-то образом и не засыпать если endflag == true
            //Unlock mutex and wait until the queue is not empty
            workers_cv_.wait(lock);
        }

        std::cout << "2 Take" << std::endl;

        if (endflag) {
            throw std::runtime_error("Cannot take task in stopped thread pool");
        }

        return TakeLocked();
    }

    void WaitAllQueue() {
        std::unique_lock<std::mutex> lock(wait_mutex_);
        completed_task_ids_cv_.wait(lock, [this]()->bool {
            std::lock_guard<std::mutex> task_lock(queue_mutex_);
            // return buffer_.empty() && last_idx_ == completed_task_ids_.size();
            std::cout << "buffer_.size() = " <<  buffer_.size() << " last_idx_ = " << last_idx_ << " completed_task_ids_.size() = " << completed_task_ids_.size() << std::endl;
            return buffer_.empty() && last_idx_ == completed_task_ids_.size();
        });
    }

    void WaitQueue(size_t task_id) {
        std::unique_lock<std::mutex> lock(wait_mutex_);
        completed_task_ids_cv_.wait(lock, [this, task_id]()->bool {
            return completed_task_ids_.find(task_id) != completed_task_ids_.end(); 
        });
    }

    bool TaskCompleteQueue(size_t task_id) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (completed_task_ids_.find(task_id) != completed_task_ids_.end())
            return true;
        return false;
    }

    void NotifyAllWorkers() {
        workers_cv_.notify_all();
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
    std::mutex wait_mutex_;
    std::condition_variable workers_cv_;
    std::condition_variable completed_task_ids_cv_;
    std::unordered_set<size_t> completed_task_ids_; 
    std::atomic<size_t> last_idx_ {0};
};

}