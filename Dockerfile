FROM ubuntu:focal

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    g++ \
    libopencv-dev \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/heuristic_optimisation

COPY include include
COPY src src
COPY CMakeLists.txt .

RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make

RUN rm -rf include
RUN rm -rf src
RUN rm CMakeLists.txt
