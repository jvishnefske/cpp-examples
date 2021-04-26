cmake_minimum_required(VERSION 3.17)

enable_testing()
set(CMAKE_CXX_STANDARD 17)
add_executable(testJunk main.cpp)
find_package(GTest REQUIRED)
target_link_libraries(testJunk GTest::gtest_main)
add_test(AllTestsInCJunk testJunk)

project(esp_audio)


#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()
#set(BOOST_ROOT ${CONAN_BOOST_ROOT})
#message("boost root ${BOOST_ROOT}")
find_package(Boost COMPONENTS thread iostreams filesystem REQUIRED)
#chrono iostreams)

add_executable(esp_audioTest tests.cpp)
#target_link_libraries(esp_audio Boost::headers)
target_compile_options(esp_audioTest PRIVATE -Werror)

find_package(GTest REQUIRED)
target_link_libraries(esp_audioTest GTest::gtest_main)
target_link_libraries(esp_audioTest Boost::iostreams)