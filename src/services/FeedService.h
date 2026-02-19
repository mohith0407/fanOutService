#pragma once

#include <memory>
#include <vector>
#include <string>

#include <sw/redis++/redis++.h>
#include "GraphService.h"
#include "../models/Post.h"
#include "../models/User.h"

#include "../include/ThreadPool.h"

namespace services {
class FeedService : public std::enable_shared_from_this<FeedService> {

public:
    FeedService(std::shared_ptr<GraphService> graphService, 
                std::shared_ptr<ThreadPool> threadPool,
                std::shared_ptr<sw::redis::Redis> redis);

    // Registers a user in the system
    void addUser(int id, std::string name);

    // Creates a post and fans it out to followers
    void postContent(int userId, std::string content);

    // Retrieves the feed for a user
    std::vector<std::shared_ptr<models::Post>> getFeedForUser(int userId, int offSet, int limit);
private:
    std::shared_ptr<GraphService> graphService_;
    std::shared_ptr<ThreadPool> threadPool_; // this is like our kitchen
    std::shared_ptr<sw::redis::Redis> redis_;

};

}