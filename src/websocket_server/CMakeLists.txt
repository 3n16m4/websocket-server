# Depedencies
CPMAddPackage(
    NAME fmt
    GIT_TAG 7.1.2
    GITHUB_REPOSITORY fmtlib/fmt
)

find_path(BOOST_ASIO_INCLUDE_DIRS "boost/asio.hpp")
find_path(BOOST_BEAST_INCLUDE_DIRS "boost/beast.hpp")

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

set(SRC_FILES
    main.cc
    CommandLineInterface.hh
    CommandLineInterface.cc
    ServerCertificate.hh
    ServerCertificate.cc
    DetectSession.hh
    DetectSession.cc
    SharedState.hh
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
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_ASIO_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_BEAST_INCLUDE_DIRS})
