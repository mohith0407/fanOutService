#include "GraphService.h"
#include <algorithm>

namespace services {

void GraphService::follow(int followerId, int followeeId) {
    if (followerId == followeeId) return; // Prevent self-follow
    // followersGraph_[followeeId].insert(followerId);
    // Redis Command: SADD user:100:followers 200
    // "SADD" adds a member to a Set (automatically handles duplicates)
    std::string key = "user:" + std::to_string(followeeId) + ":followers";
    redis_->sadd(key, std::to_string(followerId));
}

std::vector<int> GraphService::getFollowers(int userId) const {
    std::string key = "user:" + std::to_string(userId) + ":followers";
    std::vector<std::string> followersStr;
    
    // Redis Command: SMEMBERS user:100:followers
    redis_->smembers(key, std::back_inserter(followersStr));

    // Convert string IDs back to integers
    std::vector<int> followers;
    for (const auto& idStr : followersStr) {
        followers.push_back(std::stoi(idStr));
    }
    return followers;
}

}