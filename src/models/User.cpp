#include "User.h"

namespace models {
    User::User(int id, std::string name) : id_(id), name_(std::move(name)) {}
    int User::getId() const { return id_; }
    std::string User::getName() const { return name_; }

    void User::addToFeed(std::shared_ptr<Post> post){
        std::lock_guard<std::mutex> lock(feedMutex_);
        feed_.push_front(post);

        if(feed_.size() > 50) {
            feed_.pop_back();
        }
    }
    std::vector<std::shared_ptr<Post>> User::getFeedSnapshot() const {
        std::lock_guard<std::mutex> lock(feedMutex_);
        return std::vector<std::shared_ptr<Post>>(feed_.begin(), feed_.end());
    }
}