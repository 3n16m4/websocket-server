find_path(BOOST_ASIO_INCLUDE_DIRS "boost/asio.hpp")
find_path(BOOST_BEAST_INCLUDE_DIRS "boost/beast.hpp")

find_package(OpenSSL REQUIRED)

set(SRC_FILES
    main.cc
    SharedState.hh
    TCPListener.hh
    TCPListener.cc
    WebSocketSession.hh
    WebSocketSession.cc
    PlainWebSocketSession.hh
    PlainWebSocketSession.cc
    SSLWebSocketSession.hh
    SSLWebSocketSession.cc
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_ASIO_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_BEAST_INCLUDE_DIRS})
