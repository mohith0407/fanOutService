#pragma once
#include <string>
#include <chrono>
#include <ctime>

namespace models {

class Post {
public:
    Post(int id, int userId, std::string content);
    int getId() const;
    int getUserId() const;
    std::string getContent() const;
    std::time_t getTimestamp() const;
private:
    int id_;
    int userId_;
    std::string content_;
    std::time_t timestamp_;
};

}