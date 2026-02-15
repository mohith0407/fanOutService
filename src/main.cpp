#include <iostream>
#include <memory>
#include <thread>

#include "services/FeedService.h"
#include "services/GraphService.h"
#include "include/ThreadPool.h"

using namespace services;

int main() {
    // 1. Create a Thread Pool with 4 worker threads
    auto threadPool = std::make_shared<ThreadPool>(4);
    // 2. Initialize Dependency Injection
    auto graphService = std::make_shared<GraphService>();
    auto feedService = std::make_shared<FeedService>(graphService, threadPool);

    std::cout << "=== Social Fanout System Initialized(async) ===" << std::endl;

    // 3. Create Users
    feedService->addUser(101, "Elon");
    feedService->addUser(102, "Jeff");
    feedService->addUser(103, "Mark");

    // 3. Define Relationships (The Graph)
    // Jeff follows Elon
    graphService->follow(102, 101);
    // Mark follows Elon
    graphService->follow(103, 101);

    std::cout << "Users created and relationships established." << std::endl;

    std::cout << "\n[Main Thread] Requesting Post Creation..." << std::endl;
    // 4. Elon Posts (Triggering Fanout)
    std::cout << "Elon is posting..." << std::endl;
    feedService->postContent(101, "We are going to Mars!");

    std::cout << "[Main Thread] Control returned to Main! I can do other things now." << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));
    // 5. Verify Feeds
    // auto jeffFeed = feedService->getFeedForUser(102);
    // auto markFeed = feedService->getFeedForUser(103);

    // std::cout << "\n--- Jeff's Feed ---" << std::endl;
    // for (const auto& post : jeffFeed) {
    //     std::cout << "Post ID: " << post->getId() << " | Content: " << post->getContent() << std::endl;
    // }

    // std::cout << "\n--- Mark's Feed ---" << std::endl;
    // for (const auto& post : markFeed) {
    //     std::cout << "Post ID: " << post->getId() << " | Content: " << post->getContent() << std::endl;
    // }

    return 0;
}