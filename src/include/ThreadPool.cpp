#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for(size_t i = 0; i < numThreads; ++i) {
        // We use a lambda function to define what each worker does
        workers.emplace_back([this] {
            while(true) {
                std::function<void()> task;

                {
                    // 1. Lock the queue
                    std::unique_lock<std::mutex> lock(this->queueMutex);

                    // 2. Wait until a task appears OR we are told to stop
                    this->condition.wait(lock, [this]{ 
                        return this->stop || !this->tasks.empty(); 
                    });

                    // 3. If stop is requested and queue is empty, exit thread
                    if(this->stop && this->tasks.empty())
                        return;

                    // 4. Grab the task
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                } // Lock is released here automatically

                // 5. Execute the task (Cook the food)
                task();
            }
        });
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        // Add task to the queue
        tasks.push(std::move(task));
    }
    // Wake up ONE worker to handle this new task
    condition.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop = true;
    }
    // Wake up ALL workers so they can see the 'stop' flag and exit
    condition.notify_all();
    
    // Wait for all threads to finish gracefully
    for(std::thread &worker: workers) {
        worker.join();
    }
}