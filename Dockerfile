# 1. Use an official Ubuntu base image
FROM ubuntu:22.04

# Avoid interactive timezone prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# 2. Install all C++ build tools, gRPC, and Protobuf
RUN apt-get update && apt-get install -y \
    build-essential cmake git pkg-config \
    protobuf-compiler protobuf-compiler-grpc \
    libgrpc++-dev libprotobuf-dev \
    && rm -rf /var/lib/apt/lists/*

# 3. Install Hiredis (C Driver)
RUN git clone https://github.com/redis/hiredis.git && \
    cd hiredis && make && make install && cd .. && rm -rf hiredis

# 4. Install Redis-Plus-Plus (C++ Wrapper)
RUN git clone https://github.com/sewenew/redis-plus-plus.git && \
    cd redis-plus-plus && mkdir build && cd build && \
    cmake -DCMAKE_CXX_STANDARD=17 .. && make && make install && \
    ldconfig && cd ../.. && rm -rf redis-plus-plus

# 5. Copy our code into the container
WORKDIR /app
COPY . .

# 6. Build our Server
RUN mkdir -p build && cd build && \
    cmake .. && make GrpcServer

# 7. Expose the API Port
EXPOSE 50051

# 8. Run the server!
CMD ["./build/GrpcServer"]