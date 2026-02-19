cmake_minimum_required(VERSION 3.10)
project(SocialFanout)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 1. Find Libraries (Standard System Search)
find_path(REDIS_PLUS_PLUS_INCLUDE_DIR sw/redis++/redis++.h)
find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h)

find_library(REDIS_PLUS_PLUS_LIBRARY redis++)
find_library(HIREDIS_LIBRARY hiredis)

# 2. Include Headers
include_directories(src)
include_directories(${REDIS_PLUS_PLUS_INCLUDE_DIR})
include_directories(${HIREDIS_INCLUDE_DIR})

# 3. Sources
set(SOURCES
    src/models/Post.cpp
    src/models/User.cpp
    src/services/GraphService.cpp
    src/services/FeedService.cpp
    src/include/ThreadPool.cpp
)

# 4. Executables
add_executable(FanoutApp src/main.cpp ${SOURCES})
target_link_libraries(FanoutApp ${REDIS_PLUS_PLUS_LIBRARY} ${HIREDIS_LIBRARY} pthread)

# add_executable(LoadTest tests/LoadTest.cpp ${SOURCES})
# target_link_libraries(LoadTest ${REDIS_PLUS_PLUS_LIBRARY} ${HIREDIS_LIBRARY} pthread)

add_executable(RedisTest tests/RedisTest.cpp)
target_link_libraries(RedisTest ${REDIS_PLUS_PLUS_LIBRARY} ${HIREDIS_LIBRARY} pthread)