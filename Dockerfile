FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    ninja-build \
    libtbb-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Only sources
COPY CMakeLists.txt ./
COPY main.cpp ./
COPY src/ ./src/

# Make
RUN mkdir build && cd build && \
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF .. && \
    ninja

WORKDIR /app/build
ENTRYPOINT ["./ParallelColumnAnalyzer"]
