project(tests)

find_package(GTest CONFIG REQUIRED)

set (SRC_FILES
    SharedStateTest.cc
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main)

target_include_directories(${PROJECT_NAME} PUBLIC "../src/")