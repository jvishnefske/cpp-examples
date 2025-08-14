cmake_minimum_required(VERSION 3.6...3.19)

project(middleware)

set(CMAKE_CXX_STANDARD 17)

# Create library from utility files (no main function)
add_library(middleware_lib STATIC demodulator.cpp src/first.cpp src/first.h src/fft.cpp src/simple_fft.cpp src/simple_fft.h)

# cmakelists to build fft.cpp library linking fftw3 package
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW3 REQUIRED fftw3)

if(FFTW3_FOUND)
    add_library(fft SHARED src/fft.cpp)
    target_include_directories(fft PRIVATE ${FFTW3_INCLUDE_DIRS})
    target_link_libraries(fft ${FFTW3_LIBRARIES})
    target_link_directories(fft PRIVATE ${FFTW3_LIBRARY_DIRS})
    target_compile_options(fft PRIVATE ${FFTW3_CFLAGS_OTHER})
endif()

# Add demodulator test (doesn't require FFTW3)
add_executable(test_demodulator test_demodulator.cpp)
add_test(NAME test_demodulator COMMAND $<TARGET_FILE:test_demodulator>)
target_link_libraries(test_demodulator PRIVATE Catch2::Catch2WithMain middleware_lib)
