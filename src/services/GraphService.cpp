#include "GraphService.h"

namespace services {

void GraphService::follow(int followerId, int followeeId) {
    if (followerId == followeeId) return; // Prevent self-follow
    followersGraph_[followeeId].insert(followerId);
}

std::vector<int> GraphService::getFollowers(int userId) const {
    if (followersGraph_.find(userId) == followersGraph_.end()) {
        return {};
    }
    const auto& set = followersGraph_.at(userId);
    return std::vector<int>(set.begin(), set.end());
}

}