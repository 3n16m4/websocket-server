FROM ubuntu:20.10 AS builder

# Install required tools & dependencies
RUN apt-get update \
    && apt-get install software-properties-common -y \
    && rm -rf /var/lib/apt/lists/* \
    && add-apt-repository ppa:mhier/libboost-latest -y \
    && apt-get install clang -y \
    && apt-get install libc++1 -y \
    && apt-get install libc++1-11 -y \
    && apt-get install libc++-11-dev -y \
    && apt-get install libc++-dev -y \
    && apt-get install libc++abi1 -y \
    && apt-get install libc++abi1-11 -y \
    && apt-get install libc++abi-11-dev -y \
    && apt-get install libc++abi-dev -y \
    && apt-get install cmake -y \
    && apt-get install ninja-build -y \
    && apt-get install libssl-dev -y \
    && apt-get install libboost1.74-dev -y

# Build project
COPY . /cpp/src/project/
WORKDIR /cpp/src/project/

RUN export CC=/usr/bin/clang && export CXX=/usr/bin/clang++ \
    && cmake . -GNinja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build .

ENTRYPOINT ["./websocket-server", "0.0.0.0", "8080", "8081", "www", "8"]

EXPOSE 8080 8081
