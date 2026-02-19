#include <iostream>
#include <memory>
// #include <thread>

#include <sw/redis++/redis++.h>
#include "services/FeedService.h"
#include "services/GraphService.h"
#include "include/ThreadPool.h"

using namespace services;
using namespace sw::redis;

int main() {
    try {
        // 1. Connect to Redis
        auto redis = std::make_shared<Redis>("tcp://127.0.0.1:6379");
        std::cout << "Connected to Redis!" << std::endl;

        // Clean DB for testing (Optional: removes previous run's data)
        // redis->flushall(); 

        // 2. Setup Dependencies
        auto threadPool = std::make_shared<ThreadPool>(4);
        auto graphService = std::make_shared<GraphService>(redis);
        auto feedService = std::make_shared<FeedService>(graphService, threadPool, redis);

        // 3. Create Data
        feedService->addUser(1, "Mohith");
        feedService->addUser(2, "Maanas");

        // 4. maanas follows mohith
        graphService->follow(2, 1); 

        // 5. Mohith Posts
        feedService->postContent(1, "We are going to mars ");

        // Allow some time for async worker
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // 6. Check Maanas Feed
        // 1. Fetch Page 1 (Top 3 posts)
        auto page1 = feedService->getFeedForUser(2, 0, 3);
        std::cout << "--- Page 1 ---" << std::endl;
        for (auto& p : page1) std::cout << p->getContent() << std::endl;

        // 2. Fetch Page 2 (Next 3 posts)
        auto page2 = feedService->getFeedForUser(2, 3, 3);
        std::cout << "--- Page 2 ---" << std::endl;
        for (auto& p : page2) std::cout << p->getContent() << std::endl;
        std::cout << "\n--- Maanas's Feed (Fetched from Redis) ---" << std::endl;

    } catch (const Error &e) {
        std::cerr << "Redis Error: " << e.what() << std::endl;
    }

    return 0;
}