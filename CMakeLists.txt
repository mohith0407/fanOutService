cmake_minimum_required(VERSION 3.10)
project(SocialFanout)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/models/Post.cpp
    src/models/User.cpp
    src/services/GraphService.cpp
    src/services/FeedService.cpp
    src/include/ThreadPool.cpp
)

# Main Executable
add_executable(FanoutApp src/main.cpp ${SOURCES})

# Unit Tests Executable
# add_executable(RunTests tests/UnitTests.cpp ${SOURCES})

# Load Test
add_executable(LoadTest tests/LoadTest.cpp ${SOURCES})

# Link Threads Library (Required for Linux/Mac)
find_package(Threads REQUIRED)
target_link_libraries(LoadTest Threads::Threads)