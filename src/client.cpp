#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include "social_network.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using social::FeedApi;
using social::CreatePostRequest;
using social::CreatePostResponse;
using social::GetFeedRequest;
using social::GetFeedResponse;

class SocialClient {
public:
    SocialClient(std::shared_ptr<Channel> channel)
        : stub_(FeedApi::NewStub(channel)) {}

    // 1. Simulate a user making a post
    void CreatePost(int userId, const std::string& content) {
        CreatePostRequest request;
        request.set_user_id(userId);
        request.set_content(content);

        CreatePostResponse reply;
        ClientContext context;

        Status status = stub_->CreatePost(&context, request, &reply);

        if (status.ok()) {
            std::cout << "✅ [Post Success] " << reply.message() << std::endl;
        } else {
            std::cout << "❌ [Post Failed] " << status.error_code() << ": " << status.error_message() << std::endl;
        }
    }

    // 2. Simulate a user loading their feed
    void GetFeed(int userId) {
        GetFeedRequest request;
        request.set_user_id(userId);
        request.set_offset(0);
        request.set_limit(5); // Get top 5 posts

        GetFeedResponse reply;
        ClientContext context;

        Status status = stub_->GetFeed(&context, request, &reply);

        if (status.ok()) {
            std::cout << "\n📱 --- Feed for User " << userId << " ---" << std::endl;
            for (const auto& post : reply.posts()) {
                std::cout << "Post ID: " << post.id() 
                          << " | Author: " << post.author_id() 
                          << " | Content: " << post.content() << std::endl;
            }
            std::cout << "---------------------------\n" << std::endl;
        } else {
            std::cout << "❌ [Feed Failed] " << status.error_code() << ": " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<FeedApi::Stub> stub_;
};

int main(int argc, char** argv) {
    // Connect to the gRPC server we just built
    SocialClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    std::cout << "Connecting to Social Network API...\n" << std::endl;

    // 1. User 1 (Elon) posts something new
    client.CreatePost(1, "Just launched another rocket! 🚀");

    // Give the server's Async ThreadPool a tiny fraction of a second to fan out the post
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 2. User 2 (Jeff) loads his feed to see Elon's post
    client.GetFeed(2);

    return 0;
}