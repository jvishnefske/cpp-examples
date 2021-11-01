cmake_minimum_required(VERSION 3.13)
project(radio_demo)

set(CMAKE_CXX_STANDARD 11)

add_executable(radio_demo radio.cpp)
# find and link the v4l library
#find_package(v4l2 REQUIRED)
target_link_libraries(radio_demo PRIVATE v4l2)
