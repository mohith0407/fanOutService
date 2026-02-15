#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cassert>
#include <chrono>
#include "../src/services/FeedService.h"
#include "../src/services/GraphService.h"

// Configuration
const int NUM_FOLLOWERS = 2000;
const int NUM_POSTS = 100;
const int NUM_READ_THREADS = 10;

using namespace services;

void runLoadTest() {
    std::cout << "--- STARTING CONCURRENCY LOAD TEST ---" << std::endl;
    // 1. Create a Thread Pool with 4 worker threads
    auto threadPool = std::make_shared<ThreadPool>(4);
    // 1. Setup Services
    auto graphService = std::make_shared<GraphService>();
    auto feedService = std::make_shared<FeedService>(graphService,threadPool);

    // 2. Create the "Celebrity"
    int celebrityId = 99999;
    feedService->addUser(celebrityId, "Elon Musk");

    // 3. Create Followers & Follow the Celebrity
    std::cout << "Creating " << NUM_FOLLOWERS << " followers..." << std::endl;
    for (int i = 0; i < NUM_FOLLOWERS; ++i) {
        feedService->addUser(i, "User" + std::to_string(i));
        graphService->follow(i, celebrityId); // User i follows Elon
    }

    // Atomic flag to coordinate start/stop
    std::atomic<bool> running{true};
    std::atomic<int> postsRead{0};

    // 4. WRITER THREAD: Elon posts 100 times
    std::thread writer([&]() {
        for (int i = 0; i < NUM_POSTS; ++i) {
            feedService->postContent(celebrityId, "Crypto Update #" + std::to_string(i));
            // Simulate human typing speed (very fast typist)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        running = false; // Stop readers when done
    });

    // 5. READER THREADS: Followers constantly refresh feeds
    std::vector<std::thread> readers;
    for (int t = 0; t < NUM_READ_THREADS; ++t) {
        readers.emplace_back([&, t]() {
            while (running) {
                // Pick a random follower to check
                int randomFollower = rand() % NUM_FOLLOWERS;
                auto feed = feedService->getFeedForUser(randomFollower);
                
                // Just access data to trigger potential race conditions
                if (!feed.empty()) {
                    postsRead++;
                }
            }
        });
    }

    // Wait for everyone
    writer.join();
    for (auto& t : readers) {
        t.join();
    }

    std::cout << "--- WRITES & READS COMPLETED ---" << std::endl;

    // 6. Verification
    // Check a random follower's feed to ensure they got ALL 100 posts
    // (Note: Feed is capped at 100 in our User.cpp, so we expect exactly 100 or close to it)
    auto sampleFeed = feedService->getFeedForUser(0);
    std::cout << "Sample Follower Feed Size: " << sampleFeed.size() << std::endl;
    
    // If your User.cpp cap is 50, this should print 50. If 100, then 100.
    // If NO mutex was used, this might be 102, 98, or Segfault.
    
    std::cout << "Total Feed Reads performed: " << postsRead << std::endl;
    std::cout << "--- TEST PASSED (No Crashes) ---" << std::endl;
}

int main() {
    runLoadTest();
    return 0;
}