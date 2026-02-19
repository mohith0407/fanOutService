#pragma once

#include <vector>
#include <string>
#include <sw/redis++/redis++.h>

namespace services {
class GraphService {
private:
    // list: userId: set of followers id
    // std::unordered_map<int, std::unordered_set<int>> followersGraph_;
    // userid: set of following id
    // std::unordered_map<int, std::unordered_set<int>> followingGraph_;
    std::shared_ptr<sw::redis::Redis> redis_;

public:
    explicit GraphService(std::shared_ptr<sw::redis::Redis> redis) 
        : redis_(redis) {}
    void follow(int followerId, int followeeId);
    void unfollow(int followerId, int followeeId);

    // crucial for fanout to get everyone who needs to see post
    std::vector<int> getFollowers(int userId) const;
};

}