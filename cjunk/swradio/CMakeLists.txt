cmake_minimum_required(VERSION 3.13)
project(radio_demo)

set(CMAKE_CXX_STANDARD 11)

# use the CMake build system to search for the videodev2.h header and link it if found
# if the header is found, link it
set(video_include_file "linux/videodev2.h")
# check if the header exists in /usr/include
find_library(video_include_file "/usr/include/${video_include_file}")
if(VIDEO_INCLUDE_FILE)
    # if the header is found, link it
    add_executable(radio_demo radio.cpp)

    add_library(video_include_file SHARED ${video_include_file})
    target_link_libraries(radio_demo video_include_file)
else()
    message(INFO "videodev2.h not found, skipping radio_demo")
endif()
