cmake_minimum_required(VERSION 3.5)
project(untitled)
set(CMAKE_CXX_STANDARD 14)

#set(GTEST_ROOT /home/vis75817/build/googletest)
find_package(GTest)
if(GTEST_FOUND)
    # build unit tests with gtest
    message("found gtest ${GTEST_INCLUDE_DIRS} ")
    add_executable(testHello testHello.cpp)
    target_link_libraries(testHello GTest::Main)
    add_test(HelloTests testHello)
else()
    message("gtest not found inc ${GTEST_INCLUDE_DIRS}")
endif()


add_library(untitled library.cpp library.h testHello.cpp)
