# fetchcontent requries 3.11
# ubuntu 18.04 came with 3.10
cmake_minimum_required(VERSION 3.11)
include(FetchContent)
enable_testing()
set(CMAKE_CXX_STANDARD 17)

# download gtt if it is not already avaiable
#find_package(GTest REQUIRED)
FetchContent_Declare(
    Boost
    URL "https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz")
#HASH sha256=7bd7ddceec1a1dfdcbdb3e609b60d01739c38390a5f956385a12f3122049f0ca

FetchContent_MakeAvailable(Boost)
#for windows
#URL https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.zip


#consider supporting older cmake by using ExternalProject_Add(URL [])
FetchContent_Declare(
      GTest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
        )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(GTest)
find_package(GTest)
if(GTEST_FOUND)
    include(GoogleTest)
    add_library(GTest::gtest_main ALIAS gtest_main)
    add_executable(testJunk main.cpp)
    target_link_libraries(testJunk GTest::gtest_main)
    add_test(AllTestsInCJunk testJunk)
    add_executable(esp_audioTest tests.cpp)
    target_compile_options(esp_audioTest PRIVATE -Werror)
    find_package(GTest REQUIRED)
    target_link_libraries(esp_audioTest GTest::gtest_main)
    target_link_libraries(esp_audioTest Boost::iostreams)
endif()


project(esp_audio)


#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()
#set(BOOST_ROOT ${CONAN_BOOST_ROOT})
#message("boost root ${BOOST_ROOT}")
find_package(Boost COMPONENTS thread iostreams filesystem REQUIRED)
#chrono iostreams)
