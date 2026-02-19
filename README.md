# High-Performance Social Media Fanout Service (C++)

A multithreaded, asynchronous backend engine for a social network feed, written in modern C++ (C++17). This project implements a **Fanout-on-Write** architecture, pushing posts to followers' feeds in the background to ensure *O(1)* read latency for the end user.
##  Key Features
* **Asynchronous Processing:** Uses a custom C++ Thread Pool to offload heavy fanout tasks from the main execution thread.
* **Persistent Storage:** Integrated with a Dockerized **Redis** database for blazing-fast, in-memory data structures (Sets, Hashes, Lists) with disk persistence.
* **Thread Safety:** Implements strict mutex locking and `std::enable_shared_from_this` to prevent race conditions and memory leaks during concurrent operations.
* **Pagination:** Supports infinite scroll/pagination for fetching feeds efficiently.

##  Tech Stack
* **Language:** C++17
* **Database:** Redis (via Docker)
* **Build System:** CMake
* **Libraries:** `redis-plus-plus`, `hiredis`

## Project Structure
```text
├── src/
│   ├── models/        # Data Entities (User, Post)
│   ├── services/      # Business Logic (FeedService, GraphService)
│   └── include/       # Utilities (ThreadPool)
├── tests/             # Load tests & Unit tests
└── CMakeLists.txt     # Build Configuration

```

## Performance
* The system uses an Asynchronous Task Queue (Producer-Consumer pattern).
* **Write Path:** When a user posts, the request returns immediately (microseconds).
* **Background Path:** A pool of worker threads picks up the post and pushes it to follower feeds in parallel.


## Prerequisites
1. C++17 compatible compiler (GCC/Clang)
2. CMake (3.10+)
3. Docker & Docker Compose
4. `hiredis` and `redis-plus-plus` installed on your system.

## How to Build & Run
**1. Start the Redis Database**
```bash
docker-compose up -d
```
**2. Build the Project**
```bash
mkdir build && cd build
cmake ..
make

```
**3. Run the Main Application**
./FanoutApp

**4. Run the Concurrency Load Test**
./LoadTest

## Architecture Design
* Users & Posts: Stored as Redis Hash Maps (HSET).

* Follower Graph: Stored as Redis Sets (SADD, SMEMBERS).

* Timeline/Feed: Stored as Redis Lists (LPUSH, LRANGE) keeping the top recent posts readily available.

# Future Improvements

**RPC Layer:** Implement gRPC to split Services into Microservices.

