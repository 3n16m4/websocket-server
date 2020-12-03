# Depedencies
CPMAddPackage(
    NAME fmt
    GIT_TAG 7.1.2
    GITHUB_REPOSITORY fmtlib/fmt
)

CPMAddPackage(
    NAME nlohmann_json
    VERSION 3.9.1
    URL https://github.com/nlohmann/json/releases/download/v3.9.1/include.zip
    URL_HASH SHA256=6bea5877b1541d353bd77bdfbdb2696333ae5ed8f9e8cc22df657192218cad91
    OPTIONS
        "JSON_BuildTests OFF"
        "JSON_MultipleHeaders OFF"
)

if (nlohmann_json_ADDED)
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE "${nlohmann_json_SOURCE_DIR}/single_include")
endif ()

find_path(BOOST_ASIO_INCLUDE_DIRS "boost/asio.hpp")
find_path(BOOST_BEAST_INCLUDE_DIRS "boost/beast.hpp")
find_path(BOOST_UUID_INCLUDE_DIRS "boost/uuid/basic_name_generator.hpp")

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

set(SRC_FILES
    main.cc
    PacketHandler.hh
    PacketHandler.cc
    CommandLineInterface.hh
    CommandLineInterface.cc
    ServerCertificate.hh
    ServerCertificate.cc
    SharedState.hh
    SharedState.cc
    Listener.hh
    Listener.cc
    Logger.hh
    Logger.cc
    TCPSession.hh
    TCPSession.cc
    PlainTCPSession.hh
    PlainTCPSession.cc
    SSLTCPSession.hh
    SSLTCPSession.cc
    WebSocketSession.hh
    WebSocketSession.cc
    PlainWebSocketSession.hh
    PlainWebSocketSession.cc
    SSLWebSocketSession.hh
    SSLWebSocketSession.cc
    WebSocketSessionFactory.hh
    WebSocketSessionFactory.cc
    HttpSession.hh
    HttpSession.cc
    PlainHttpSession.hh
    PlainHttpSession.cc
    SSLHttpSession.hh
    SSLHttpSession.cc
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
    fmt::fmt-header-only
    nlohmann_json
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_ASIO_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_BEAST_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_UUID_INCLUDE_DIRS})
