#include "thread_pool.hpp"

#include <iostream>

namespace MT {
    void ThreadPool::workerMain() {
        while (canRun) {
            std::unique_ptr<Task> task;
            {
                std::unique_lock<std::mutex> lock(tasksLock);
                
                tasksExist.wait(lock, [this] {return !tasks.empty() || !canRun;});

                if (!canRun) {
                    break;
                }

                task = std::move(tasks.front());
                tasks.pop();
            }

            task->execute();
        }
    }

    ThreadPool::ThreadPool(int numThreads) {
        for (int i = 0; i < numThreads; ++i) {
            workers.emplace_back(&ThreadPool::workerMain, this);
        }
    }
    
    void ThreadPool::addTask(std::unique_ptr<Task> task) {
        {
            std::unique_lock<std::mutex> lock(tasksLock);
            tasks.push(std::move(task));
        }
        tasksExist.notify_one();
    }

    void ThreadPool::clearTasks() {
        std::unique_lock<std::mutex> lock(tasksLock);
        tasks = std::queue<std::unique_ptr<Task>>();
    }

    size_t ThreadPool::getTasksNumber() {
        std::unique_lock<std::mutex> lock(tasksLock);
        return tasks.size();
    }

    ThreadPool::~ThreadPool() {
        canRun = false;
        tasksExist.notify_all();
        for (auto& worker : workers) {
            worker.join();
            std::cerr << "Thread terminated\n";
        }
    }
}