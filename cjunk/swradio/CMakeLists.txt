cmake_minimum_required(VERSION 3.13)
project(radio_demo)

set(CMAKE_CXX_STANDARD 11)
find_package(videodev)
# if videodev found
if(videodev_FOUND)
    find_package(v4l2)
    # if v4l2 found
    if(v4l2_FOUND)
        # v4l2 is used for camera
        add_executable(radio_demo radio.cpp)
        target_link_libraries(radio_demo PRIVATE v4l2)
    endif()
endif()

