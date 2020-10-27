find_path(BOOST_BEAST_INCLUDE_DIRS "boost/beast.hpp")

set(SRC_FILES
    main.cc
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_BEAST_INCLUDE_DIRS})
