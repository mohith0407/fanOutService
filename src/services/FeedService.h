#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include "GraphService.h"
#include "../models/Post.h"
#include "../models/User.h"

#include "../include/ThreadPool.h"

namespace services {
class FeedService : public std::enable_shared_from_this<FeedService> {

public:
    FeedService(std::shared_ptr<GraphService> graphService, 
                std::shared_ptr<ThreadPool> threadPool);

    // Registers a user in the system
    void addUser(int id, std::string name);

    // Creates a post and fans it out to followers
    void postContent(int userId, std::string content);

    // Retrieves the feed for a user
    std::vector<std::shared_ptr<models::Post>> getFeedForUser(int userId);
private:
    std::shared_ptr<GraphService> graphService_;
    std::shared_ptr<ThreadPool> threadPool_; // this is like our kitchen
    
    // In-memory "Database"
    std::unordered_map<int, std::shared_ptr<models::User>> userRepo_;
    std::unordered_map<int, std::shared_ptr<models::Post>> postRepo_;
    
    int nextPostId_ = 1; // Auto-increment ID simulator
    mutable std::mutex repoMutex_; // protects postRepo_ during writes
};

}