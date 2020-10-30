cmake_minimum_required(VERSION 3.15)

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif ()

project(websocket-server LANGUAGES CXX)

include(cmake/Cache.cmake)
include(cmake/CheckAndAddFlag.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Doxygen.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/StandardProjectSettings.cmake)
include(cmake/StaticAnalyzers.cmake)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

enable_sanitizers(project_options)
enable_doxygen()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_PCH "Enable Precompiled Headers" ON)
if(ENABLE_PCH)
    target_precompile_headers(
            project_options
            INTERFACE
            <vector>
            <string>
            <map>
            <utility>)
endif()

# Architecture detection and arch specific settings
message(STATUS "Detected architecture: ${CMAKE_SYSTEM_PROCESSOR}")

# Enable compiler specific flags
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using Clang
    if (UNIX AND NOT APPLE)
        message(STATUS "Compiling under GNU/Linux with LLVM/CLang")
        check_and_add_flag(LIBC++ -stdlib=libc++)
        check_and_add_flag(FOMIT_FRAME_POINTER -fno-omit-frame-pointer RELEASE_ONLY)
        check_and_add_flag(FLTO -flto=thin)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++abi -stdlib=libc++")
    elseif (APPLE)
        message(STATUS "Compiling under macOS with LLVM/CLang")
        check_and_add_flag(LIBC++ -stdlib=libc++)
        check_and_add_flag(FOMIT_FRAME_POINTER -fno-omit-frame-pointer RELEASE_ONLY)
        check_and_add_flag(FLTO -flto=thin RELEASE_ONLY)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++abi")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (APPLE)
        message(STATUS "Compiling under macOS with GNU/GCC")
        check_and_add_flag(FOMIT_FRAME_POINTER -fomit-frame-pointer RELEASE_ONLY)
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    message(FATAL_ERROR "Intel Compiler is not supported.")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Compiling under MSVC")
    # Needed for boost1.69
    # Avoid that Python (pyerrors.h) defines snprintf and vsnprintf
    if (MSVC AND NOT MSVC_VERSION VERSION_LESS 1900)
        # check_and_add_flag(VISIBILITY_HIDDEN -fvisibility=hidden)
        # Disable warnings coming from 3rd-party libraries.
        add_compile_options(/experimental:external /external:I $ENV{VCPKG_ROOT}/installed/$ENV{VCPKG_DEFAULT_TRIPLET}/include /external:W0 /external:templates- /external:anglebrackets)
        # Allow big object files, required for boost.
        add_compile_options(/bigobj)
        add_definitions(-DBOOST_UUID_FORCE_AUTO_LINK)
        add_definitions(-D_WIN32_WINNT=0x0601)
        add_definitions(-DHAVE_SNPRINTF)
        add_definitions(-DWIN32_LEAN_AND_MEAN)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        add_definitions(-DNOMINMAX)
    endif()
endif ()

# Force exes/libs to output in the build/ folder, rather than having each output
# in its own build/[module]/ folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src/websocket_server)

set(ENABLE_TESTING OFF)

if(ENABLE_TESTING)
    enable_testing()
    message("Building Tests.")
    add_subdirectory(test)
endif()