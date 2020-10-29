find_path(BOOST_ASIO_INCLUDE_DIRS "boost/asio.hpp")
find_path(BOOST_BEAST_INCLUDE_DIRS "boost/beast.hpp")

set(SRC_FILES
    main.cc
    SharedState.hh
    TCPListener.hh
    TCPListener.cc
    WebsocketSession.hh
    WebsocketSession.cc
    WebsocketSessionPlain.hh
    WebsocketSessionPlain.cc
    WebsocketSessionSSL.hh
    WebsocketSessionSSL.cc
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_ASIO_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_BEAST_INCLUDE_DIRS})
