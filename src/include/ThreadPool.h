#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

// This is a generic utility class. It doesn't know about "Posts" or "Users". It just knows how to execute tasks.

class ThreadPool {
public:
    // Constructor: Launches 'numThreads' workers
    explicit ThreadPool(size_t numThreads);

    // Destructor: Stops all threads safely
    ~ThreadPool();

    // Add a task to the queue
    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};