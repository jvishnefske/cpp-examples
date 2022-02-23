cmake_minimum_required(VERSION 3.6...3.19)

project(middleware)

set(CMAKE_CXX_STANDARD 17)

add_executable(middleware main.cpp src/first.cpp src/first.h src/fft.cpp src/stupid_fft.cpp src/stupid_fft.h src/dump_serial.cpp src/nonworking_json.cpp)

# cmakelists to build fft.cpp library linking fftw3 package
add_library(fft SHARED fft.cpp)
find_package(FFTW3 REQUIRED)
include_directories(${FFTW3_INCLUDE_DIRS})
target_link_libraries(fft ${FFTW3_LIBRARIES})