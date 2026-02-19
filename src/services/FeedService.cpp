#include "FeedService.h"
#include <iostream>

namespace services {

FeedService::FeedService(std::shared_ptr<GraphService> graphService,
                         std::shared_ptr<ThreadPool> threadPool,
                         std::shared_ptr<sw::redis::Redis> redis)
    : graphService_(graphService), threadPool_(threadPool), redis_(redis) {}

void FeedService::addUser(int id, std::string name) {
   // Redis: HSET user:1 name "Mohith"
    redis_->hset("user:" + std::to_string(id), "name", name);
}

void FeedService::postContent(int userId, std::string content) {

    // 1. Generate a Post ID (In a real app, use UUID or Redis INCR)
    long long newPostId = redis_->incr("global:post:id");
    
    // 2. Save Post Data to Redis (Hash Map)
    // HSET post:100 content "Hello" author 1
    std::string postKey = "post:" + std::to_string(newPostId);
    redis_->hset(postKey, {
        std::make_pair("content", content),
        std::make_pair("author", std::to_string(userId)),
        std::make_pair("timestamp", std::to_string(std::time(nullptr)))
    });

    std::cout << "[Main] Post " << newPostId << " saved to Redis.\n";

    // 3. ASYNC FANOUT
    auto self = shared_from_this();
    threadPool_->enqueue([self, userId, newPostId]() {
        // Get followers from Redis
        std::vector<int> followers = self->graphService_->getFollowers(userId);
        
        for (int followerId : followers) {
            std::string feedKey = "user:" + std::to_string(followerId) + ":feed";
            
            // Redis Pipeline is efficient for batch jobs, but simple commands work too.
            // LPUSH: Add to top of feed
            self->redis_->lpush(feedKey, std::to_string(newPostId));
            
            // LTRIM: Keep only top 100 posts (Save space)
            self->redis_->ltrim(feedKey, 0, 99);
        }
        std::cout << "[Worker] Fanout for Post " << newPostId << " complete.\n";
    });
}

std::vector<std::shared_ptr<models::Post>> FeedService::getFeedForUser(int userId, int offset, int limit) {
    std::string feedKey = "user:" + std::to_string(userId) + ":feed";
    std::vector<std::string> postIds;
    
    // Redis LRANGE uses inclusive indices [start, stop]
    // If offset=0, limit=10 -> start=0, stop=9
    int start = offset;
    int stop = offset + limit - 1;
    
    redis_->lrange(feedKey, start, stop, std::back_inserter(postIds));

    std::vector<std::shared_ptr<models::Post>> feed;
    for (const auto& pidStr : postIds) {
        std::string postKey = "post:" + pidStr;
        
        // OPTIMIZATION: Use HMGET (Multi-Get) to fetch fields in one network packet
        std::vector<std::string> fields = {"content", "author"};
        std::vector<std::optional<std::string>> values;
        redis_->hmget(postKey, fields.begin(), fields.end(), std::back_inserter(values));
        
        // values[0] is content, values[1] is author
        if (values[0] && values[1]) {
            int pid = std::stoi(pidStr);
            int authorId = std::stoi(*values[1]);
            feed.push_back(std::make_shared<models::Post>(pid, authorId, *values[0]));
        }
    }
    return feed;
}
}