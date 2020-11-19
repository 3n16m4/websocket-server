# websocket-server
A fully asynchronous, multi-threaded, lightweight WebSocket server written in C++ with Boost.Beast.

[![Ubuntu](https://github.com/3n16m4/websocket-server/workflows/Ubuntu/badge.svg)](https://github.com/3n16m4/websocket-server/actions?query=workflow%3AUbuntu)
[![Windows](https://github.com/3n16m4/websocket-server/workflows/Windows/badge.svg)](https://github.com/3n16m4/websocket-server/actions?query=workflow%3AWindows)
[![OS/X](https://github.com/3n16m4/websocket-server/workflows/MacOS/badge.svg)](https://github.com/3n16m4/websocket-server/actions?query=workflow%3AMacOS)
[![C++17](https://img.shields.io/badge/standard-C%2B%2B%2017-blue.svg?logo=C%2B%2B)](https://isocpp.org/)

## Supported Platforms
- Windows x64 / x86 / ARM
- GNU / Linux x64 / x86 / ARM
- macOS x64

## Building
### Requirements
- CMake
- ninja (Optional, for faster builds)
- LLVM CLang, GNU/GCC Compiler or MSVC with C++17 support.
- boost-beast
- nlohmann_json
- fmtlib

> **ProTip:** Use Arch Linux to grab the latest compilers.

## Getting started
### Using the Docker image (recommended)
The easiest way to setup a working webserver is to use the provided Dockerfile.

*GNU / Linux and macOS*
```
> ./run.sh
```

*Windows*
```
> run.bat
```

### Building the dependencies
Clone the [vcpkg repo](https://github.com/microsoft/vcpkg) and booststrap it.
```
> git clone https://github.com/Microsoft/vcpkg.git
> cd vcpkg
```
*GNU / Linux*
```
LINUX> ./bootstrap-vcpkg.sh
LINUX> ./vcpkg integrate install
LINUX> echo >> "export PATH=/${HOME}/src/vcpkg/:$PATH" ~/.bashrc
LINUX> echo >> "export VCPKG_ROOT=/$HOME/src/vcpkg" ~/.bashrc
LINUX> cd <websocket-server>
LINUX> vcpkg --triplet x64-linux install @response_file.txt --clean-after-build
```

*macOS*
```
UNIX> ./bootstrap-vcpkg.sh
UNIX> ./vcpkg integrate install
UNIX> echo >> "export PATH=/${HOME}/src/vcpkg/:$PATH" ~/.zshrc
UNIX> echo >> "export VCPKG_ROOT=/$HOME/src/vcpkg" ~/.zshrc
UNIX> cd <websocket-server>
UNIX> vcpkg --triplet x64-osx install @response_file.txt --clean-after-build
```
*Windows*
```
PS> .\bootstrap-vcpkg.bat
PS> .\vcpkg integrate install
PS> $env:Path += ";C:\Users\YourUserName\src\vcpkg"
PS> cd <websocket-server>
PS> vcpkg --triplet x64-windows install @response_file.txt --clean-after-build
```

### Installing the prerequisites
*GNU / Linux (Arch Linux)*
```
sudo pacman -S git cmake ninja base-devel clang
```

*Windows*

Just grab the latest [Visual Studio](https://visualstudio.microsoft.com/vs/) build to get the latest stable release of MSVC.

cmake, ninja and git should be included in Visual Studio >= 2019.

## Dependencies
- Boost.Asio (https://github.com/chriskohlhoff/asio, Christopher M. Kohlhoff)
- Boost.Beast (https://github.com/boostorg/beast, Vinnie Falco)
- nlohmann_json (https://github.com/nlohmann/json, Niels Lohmann)
- fmtlib (https://github.com/fmtlib/fmt, Victor Zverovich)
