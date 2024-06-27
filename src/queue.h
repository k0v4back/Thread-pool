#pragma once

#include <iostream>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <unordered_set>
#include <future>
#include <unordered_map>

namespace tp {

template <typename T>
class UnboundedBlockingMPMCQueue {
public:
    template <typename Func, typename ...Args, typename ...FuncTypes>
    size_t Put(Func(*func)(FuncTypes...), Args&&... args) {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        size_t task_id = last_idx_++;
        tasks_info_[task_id] = TaskInfo();
        buffer_.emplace_back(Task(func, std::forward<Args>(args)...), task_id);

        //Put one element in queue and woke up one thread,
        //which may have been waiting for an element
        workers_cv_.notify_one();

        return task_id;
    }

    void Take(std::atomic<bool>& end_flag) {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        while (buffer_.empty()) {
            //Unlock mutex and wait until the queue is not empty and thread pool is still running
            workers_cv_.wait(lock, [this, &end_flag]()->bool {
                return !buffer_.empty() || end_flag;
            });
        }

        TakeLocked();
    }

    void WaitAllQueue() {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        completed_task_ids_cv_.wait(lock, [this]()->bool {
            std::lock_guard<std::mutex> task_lock(completed_task_ids_mutex_);
            return buffer_.empty() && cnt_completed_tasks_ == last_idx_;
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

    void NotifyAllTasks() {
        workers_cv_.notify_all();
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
        std::pair<Task, uint64_t> task = std::move(buffer_.front());
        buffer_.pop_front();

        task.first();

        if (task.first.has_result()) {
            tasks_info_[task.second].result = task.first.get_result();
        }
        tasks_info_[task.second].status = TaskStatus::completed;
        ++cnt_completed_tasks_;

        completed_task_ids_.insert(task.second);
        completed_task_ids_cv_.notify_all();
    }

private:
    std::deque<std::pair<T, size_t>> buffer_; //Tasks and number of task
    std::mutex queue_mutex_;
    std::mutex completed_task_ids_mutex_;
    std::condition_variable workers_cv_;
    std::condition_variable completed_task_ids_cv_;
    std::unordered_set<size_t> completed_task_ids_; 
    std::atomic<size_t> last_idx_ {0};
    std::atomic<uint64_t> cnt_completed_tasks_ {0};
    std::unordered_map<uint64_t, TaskInfo> tasks_info_;
};

}