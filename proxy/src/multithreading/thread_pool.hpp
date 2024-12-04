#pragma once

#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace MT {
    class Task {
    public:
        virtual void execute() = 0;
        virtual ~Task() = default;
    };

    class ThreadPool {
        void workerMain();
    public:
        ThreadPool(int numThreads);

        void addTask(std::unique_ptr<Task> task);
        void clearTasks();
        size_t getTasksNumber();

        ~ThreadPool();
    private:
        std::vector<std::thread> workers;
        std::queue<std::unique_ptr<Task>> tasks;

        std::mutex tasksLock;
        std::condition_variable tasksExist;

        std::atomic_bool canRun = true;
    };
} // namespace MultiThreading