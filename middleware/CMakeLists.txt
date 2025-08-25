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

# Add individual tests for library source files to ensure coverage
add_executable(test_first src/first.cpp src/first.h)
target_compile_definitions(test_first PRIVATE STANDALONE_TEST=1)
add_test(NAME test_first COMMAND test_first)
set_tests_properties(test_first PROPERTIES TIMEOUT 5)

add_executable(test_simple_fft src/simple_fft.cpp src/simple_fft.h)
target_compile_definitions(test_simple_fft PRIVATE STANDALONE_TEST=1)  
add_test(NAME test_simple_fft COMMAND test_simple_fft)
set_tests_properties(test_simple_fft PROPERTIES TIMEOUT 5)

# Add standalone demodulator test
add_executable(test_demodulator_standalone demodulator.cpp)
target_compile_definitions(test_demodulator_standalone PRIVATE STANDALONE_TEST=1)
add_test(NAME test_demodulator_standalone COMMAND test_demodulator_standalone)
set_tests_properties(test_demodulator_standalone PROPERTIES TIMEOUT 5)

# Test the fft.cpp library source if FFTW3 is available
if(FFTW3_FOUND)
    add_executable(test_fft_standalone src/fft.cpp)
    target_include_directories(test_fft_standalone PRIVATE ${FFTW3_INCLUDE_DIRS})
    target_link_libraries(test_fft_standalone ${FFTW3_LIBRARIES})
    target_link_directories(test_fft_standalone PRIVATE ${FFTW3_LIBRARY_DIRS})
    target_compile_options(test_fft_standalone PRIVATE ${FFTW3_CFLAGS_OTHER})
    target_compile_definitions(test_fft_standalone PRIVATE STANDALONE_TEST=1)
    add_test(NAME test_fft_standalone COMMAND test_fft_standalone)
    set_tests_properties(test_fft_standalone PROPERTIES TIMEOUT 5)
endif()
