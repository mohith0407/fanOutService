# High-Performance Social Media Fanout Service (C++)

A high-concurrency, in-memory backend service simulating the "Fanout-on-Write" architecture used by Twitter/Instagram. Built from scratch in C++17.

##  Key Features
* **Fanout-on-Write Architecture:** Optimizes for read-heavy workloads (O(1) read time).
* **Asynchronous Processing:** Custom `ThreadPool` implementation to handle fanout tasks in background threads, ensuring non-blocking user interactions.
* **Thread Safety:** robust `std::mutex` and `std::lock_guard` implementation to prevent race conditions during concurrent feed updates.
* **Memory Management:** Extensive use of Smart Pointers (`std::shared_ptr`) for RAII-compliant memory safety (no manual `delete`).
* **Graph Data Structure:** Adjacency lists (Hash Maps of Sets) to model follower relationships with O(1) lookup time.

##  Tech Stack
* **Language:** C++17
* **Build System:** CMake
* **Concurrency:** `std::thread`, `std::mutex`, `std::condition_variable`, `std::atomic`
* **Testing:** Custom Load Testing script simulating high-concurrency writes.

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

## How to Build & Run
**Prerequisites:** C++ Compiler (GCC/Clang), CMake.

```bash
mkdir build && cd build
cmake ..
make

```
# Run the Main Application
./FanoutApp

# Run the Concurrency Load Test
./LoadTest

# Future Improvements

**Persistence:** Replace in-memory maps with Redis/PostgreSQL.

**RPC Layer:** Implement gRPC to split Services into Microservices.

**Pagination:** Implement cursor-based pagination for feeds.

