#include "Post.h"

namespace models {
    Post::Post(int id, int userId, std::string content)
        : id_(id), userId_(userId), content_(std::move(content)){
            timestamp_ = std::time(nullptr);
    }
    int Post::getId() const { return id_; }
    int Post::getUserId() const { return userId_; }
    std::string Post::getContent() const { return content_; }
    std::time_t Post::getTimestamp() const { return timestamp_; }
}