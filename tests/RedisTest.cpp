#include <iostream>
#include <sw/redis++/redis++.h>

using namespace sw::redis;

int main() {
    try {
        // Connect to Redis running in Docker (localhost:6379)
        auto redis = Redis("tcp://127.0.0.1:6379");

        // Simple Ping
        std::cout << "Ping: " << redis.ping() << std::endl;

        // Write Data
        redis.set("test_key", "Hello from WSL2!");
        
        // Read Data
        auto val = redis.get("test_key");
        if (val) {
            std::cout << "Retrieved: " << *val << std::endl;
        }

        std::cout << "--- REDIS CONNECTION SUCCESSFUL ---" << std::endl;
    } catch (const Error &e) {
        std::cerr << "Redis Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}