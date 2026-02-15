#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace services {
class GraphService {
private:
    // list: userId: set of followers id
    std::unordered_map<int, std::unordered_set<int>> followersGraph_;

    // userid: set of following id
    std::unordered_map<int, std::unordered_set<int>> followingGraph_;
public:
    void follow(int followerId, int followeeId);
    void unfollow(int followerId, int followeeId);

    // crucial for fanout to get everyone who needs to see post
    std::vector<int> getFollowers(int userId) const;
};

}