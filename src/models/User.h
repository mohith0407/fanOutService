#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <list>
#include <string>

#include "Post.h"

namespace models {
class User {
public:
    User(int id, std::string name);
    int getId() const;
    std::string getName() const;

    // The user's home feed (Timeline)
    // We store Post pointers. Using std::list for O(1) front insertion.
    void addToFeed(std::shared_ptr<Post> post);
    std::vector<std::shared_ptr<Post>> getFeedSnapshot() const;
private:
    int id_;
    std::string name_;
    std::list<std::shared_ptr<Post>> feed_;
    mutable std::mutex feedMutex_;
};
}