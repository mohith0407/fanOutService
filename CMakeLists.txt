cmake_minimum_required(VERSION 3.10)
project(SocialFanout)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- 1. FIND REDIS ---
find_path(REDIS_PLUS_PLUS_INCLUDE_DIR sw/redis++/redis++.h)
find_library(REDIS_PLUS_PLUS_LIBRARY redis++)
find_library(HIREDIS_LIBRARY hiredis)

# --- 2. FIND GRPC & PROTOBUF ---
# Ubuntu's apt-get installs these globally, so standard find_package works
find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GRPC REQUIRED grpc++)
pkg_check_modules(PROTOBUF REQUIRED protobuf)

find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin)
if(NOT GRPC_CPP_PLUGIN)
    set(GRPC_CPP_PLUGIN "/usr/bin/grpc_cpp_plugin")
endif()

message(STATUS "Using gRPC C++ Plugin: ${GRPC_CPP_PLUGIN}")

# --- 3. GENERATE C++ CODE FROM .proto ---
set(PROTO_FILE "${CMAKE_SOURCE_DIR}/proto/social_network.proto")
set(PROTO_SRC "${CMAKE_BINARY_DIR}/social_network.pb.cc")
set(PROTO_HDR "${CMAKE_BINARY_DIR}/social_network.pb.h")
set(GRPC_SRC "${CMAKE_BINARY_DIR}/social_network.grpc.pb.cc")
set(GRPC_HDR "${CMAKE_BINARY_DIR}/social_network.grpc.pb.h")

# This command runs `protoc` automatically when you type `make`
add_custom_command(
    OUTPUT ${PROTO_SRC} ${PROTO_HDR} ${GRPC_SRC} ${GRPC_HDR}
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
    ARGS --grpc_out=${CMAKE_BINARY_DIR}
         --cpp_out=${CMAKE_BINARY_DIR}
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
         -I ${CMAKE_SOURCE_DIR}/proto
         ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
)

# Include directories
include_directories(src)
include_directories(${REDIS_PLUS_PLUS_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR}) # Where generated .pb.h files live
include_directories(${GRPC_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIRS})

# Source files (Our internal logic)
set(SOURCES
    src/models/Post.cpp
    src/models/User.cpp
    src/services/GraphService.cpp
    src/services/FeedService.cpp
    src/include/ThreadPool.cpp
)

# --- 4. EXECUTABLES ---

# The new gRPC Server Executable
add_executable(GrpcServer src/server.cpp ${SOURCES} ${PROTO_SRC} ${GRPC_SRC})
target_link_libraries(GrpcServer 
    ${REDIS_PLUS_PLUS_LIBRARY} 
    ${HIREDIS_LIBRARY}
    ${GRPC_LIBRARIES} 
    ${PROTOBUF_LIBRARIES}  
    pthread
)

# --- The gRPC Client Executable ---
add_executable(GrpcClient src/client.cpp ${PROTO_SRC} ${GRPC_SRC})

# Notice we DO NOT link Redis here. The client shouldn't touch the database directly!
target_link_libraries(GrpcClient 
    ${GRPC_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    pthread
)

# add_executable(FanoutApp src/main.cpp ${SOURCES})
# target_link_libraries(FanoutApp ${REDIS_PLUS_PLUS_LIBRARY} ${HIREDIS_LIBRARY} pthread)

# add_executable(LoadTest tests/LoadTest.cpp ${SOURCES})
# target_link_libraries(LoadTest ${REDIS_PLUS_PLUS_LIBRARY} ${HIREDIS_LIBRARY} pthread)

# add_executable(RedisTest tests/RedisTest.cpp)
# target_link_libraries(RedisTest ${REDIS_PLUS_PLUS_LIBRARY} ${HIREDIS_LIBRARY} pthread)