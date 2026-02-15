#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include "../src/services/FeedService.h"
#include "../src/services/GraphService.h"

using namespace services;
using namespace models;

void testFanoutMechanism() {
    std::cout << "Running Test: Fanout Mechanism... ";

    // Setup
    // 1. Create a Thread Pool with 4 worker threads
    auto threadPool = std::make_shared<ThreadPool>(4);
    auto graphService = std::make_shared<GraphService>();
    auto feedService = std::make_shared<FeedService>(graphService, threadPool);

    // Create Users
    int aliceId = 1;
    int bobId = 2;
    feedService->addUser(aliceId, "Alice");
    feedService->addUser(bobId, "Bob");

    // Bob follows Alice
    graphService->follow(bobId, aliceId);

    // Alice posts
    feedService->postContent(aliceId, "Hello World!");

    // Check Bob's Feed
    auto bobsFeed = feedService->getFeedForUser(bobId);
    
    // Assertions
    assert(bobsFeed.size() == 1);
    assert(bobsFeed[0]->getContent() == "Hello World!");
    assert(bobsFeed[0]->getUserId() == aliceId);

    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "--- STARTING TESTS ---" << std::endl;
    testFanoutMechanism();
    std::cout << "--- ALL TESTS PASSED ---" << std::endl;
    return 0;
}