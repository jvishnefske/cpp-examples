cmake_minimum_required(VERSION 3.17)
project(esp_audio)

set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
set(BOOST_ROOT ${CONAN_BOOST_ROOT})
message("boost root ${BOOST_ROOT}")
find_package(Boost COMPONENTS thread iostreams filesystem REQUIRED)
#chrono iostreams)

add_executable(esp_audio main.cpp)
#target_link_libraries(esp_audio Boost::headers)
target_compile_options(esp_audio PRIVATE -Werror)

find_package(GTest REQUIRED)
add_executable(tests tests.cpp)
target_link_libraries(tests GTest::gtest_main)
target_link_libraries(tests Boost::iostreams)