#include "FeedService.h"
#include <iostream>

namespace services {

FeedService::FeedService(std::shared_ptr<GraphService> graphService,
                         std::shared_ptr<ThreadPool> threadPool)
    : graphService_(graphService), threadPool_(threadPool) {}

void FeedService::addUser(int id, std::string name) {
    std::lock_guard<std::mutex> lock(repoMutex_);
    userRepo_[id] = std::make_shared<models::User>(id, name);
}

void FeedService::postContent(int userId, std::string content) {

    // 1. FAST PART: Create Post and Save to DB (Synchronous)
    // The user needs to know "Post Created" immediately.
    int newPostId;
    std::shared_ptr<models::Post> newPost;
    
    {
        std::lock_guard<std::mutex> lock(repoMutex_); // LOCK THE DB

        if (userRepo_.find(userId) == userRepo_.end()) {
            std::cerr << "Error: User " << userId << " does not exist." << std::endl;
            return;
        }
        // 1. create a post
        newPostId = nextPostId_++;
        newPost = std::make_shared<models::Post>(newPostId, userId, content);
        // 2. save to db
        postRepo_[newPostId] = newPost;
        
        // 3. Add to author's feed immediately so they see their own post
        userRepo_[userId]->addToFeed(newPost);
    } // unlock the db
    
    std::cout << "[Main Thread] Post " << newPostId << " saved. Returning response to user.\n";

    // 2. SLOW PART: Fanout (Asynchronous)
    // We package the heavy work into a lambda and give it to the ThreadPool.
    // 'this' is captured to access member variables.
    // 'userId' and 'newPost' are captured by value.

    // This ensures FeedService cannot be destroyed until the lambda finishes!
    auto self = shared_from_this();
    threadPool_->enqueue([self, userId, newPost]() {
        // This block runs on a Background Thread!
        
        std::vector<int> followers = self->graphService_->getFollowers(userId);
        
        for (int followerId : followers) {
            // Simulate network latency (e.g., 10ms per follower)
            // std::this_thread::sleep_for(std::chrono::milliseconds(10)); 

            std::unique_lock<std::mutex> lock(self->repoMutex_);
            if (self->userRepo_.find(followerId) != self->userRepo_.end()) {
                auto follower = self->userRepo_[followerId];
                lock.unlock(); // Unlock early before doing the heavy addToFeed
                
                // User::addToFeed is already thread-safe (has its own mutex)
                follower->addToFeed(newPost);
            }
        }
        std::cout << "[Background Worker] Fanout for Post " << newPost->getId() << " complete.\n";
    });
}

std::vector<std::shared_ptr<models::Post>> FeedService::getFeedForUser(int userId) {
    std::lock_guard<std::mutex> lock(repoMutex_);
    if (userRepo_.find(userId) == userRepo_.end()) return {};
    
    // Convert list to vector for return
    // const auto& listFeed = userRepo_[userId]->getFeedSnapshot();
    // return std::vector<std::shared_ptr<models::Post>>(listFeed.begin(), listFeed.end());
    
    // using threadpool
    return userRepo_[userId]->getFeedSnapshot();
}

}