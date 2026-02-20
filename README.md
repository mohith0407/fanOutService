# High-Performance Social Media Fanout Service (C++)

A distributed, high-concurrency backend service for a social network feed. Built in modern C++ (C++17), this project implements a **Fanout-on-Write** architecture and exposes a high-performance **gRPC API**. The entire system is containerized using Docker.

##  Key Features

* **gRPC API Layer:** Replaced standard local function calls with a high-performance, strongly-typed gRPC network interface using Protocol Buffers (`.proto`).
* **Asynchronous Fanout:** Uses a custom C++ `ThreadPool` to push posts to follower feeds in the background, ensuring $O(1)$ read latency for end users.
* **Persistent Storage:** Integrated with **Redis** for blazing-fast in-memory data structures (Sets, Hashes, Lists) with persistent disk backups.
* **Dockerized Infrastructure:** The C++ Server and Redis database are containerized and orchestrated via `docker-compose` for seamless cross-platform deployment.
* **Pagination:** Supports offset-based pagination for timeline feeds.

##  Tech Stack

* **Language:** C++17
* **API Framework:** gRPC & Protocol Buffers (Protobuf)
* **Database:** Redis (via `redis-plus-plus` & `hiredis`)
* **DevOps:** Docker, Docker Compose, CMake

## Project Structure

```text
├── proto/             # gRPC Protocol Buffer definitions (.proto)
├── src/
│   ├── models/        # Data Entities
│   ├── services/      # Business Logic (FeedService, GraphService)
│   ├── include/       # Utilities (ThreadPool)
│   ├── server.cpp     # gRPC Server Entrypoint
│   └── client.cpp     # Dummy C++ gRPC Client for testing
├── docker-compose.yml # Container orchestration
├── Dockerfile         # C++ Ubuntu build environment
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

## Testing the API

You can test the endpoints using Postman (which supports gRPC) or by building the local C++ test client:

```bash
mkdir build && cd build
cmake ..
make GrpcClient
./GrpcClient

```

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

