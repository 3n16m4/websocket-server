FROM ubuntu:20.10 AS builder

# Install required tools
RUN apt-get update \
    && apt-get install clang -y \
    && apt-get install libc++1 -y \
    && apt-get install libc++1-11 -y \
    && apt-get install libc++-11-dev -y \
    && apt-get install libc++-dev -y \
    && apt-get install libc++abi1 -y \
    && apt-get install libc++abi1-11 -y \
    && apt-get install libc++abi-11-dev -y \
    && apt-get install libc++abi-dev -y \
    && apt-get install build-essential -y \
    && apt-get install g++ -y \
    && apt-get install cmake -y \
    && apt-get install ninja-build -y \
    && apt-get install git -y \
    && apt-get install wget -y \
    && apt-get install curl -y \
    && apt-get install tar -y \
    && apt-get install zip -y \
    && apt-get install unzip -y \
    && ldconfig

# Install vcpkg
RUN cd /home \
    && git clone https://github.com/microsoft/vcpkg \
    && ./vcpkg/bootstrap-vcpkg.sh \
    && ./vcpkg/vcpkg integrate install

# Build project
COPY . /cpp/src/project/
WORKDIR /cpp/src/project/

RUN export CC=/usr/bin/clang && export CXX=/usr/bin/clang++ \
    && ls -al \
    && /home/vcpkg/vcpkg --triplet x64-linux install @response_file.txt \
    && export VCPKG_ROOT=/home/vcpkg \
    && cmake . -GNinja \
    && cmake --build .

ENTRYPOINT ["./websocket-server", "0.0.0.0", "8080", "8081", "www", "8"]

EXPOSE 8080 8081

