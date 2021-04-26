
enable_testing()
set(CMAKE_CXX_STANDARD 17)
add_executable(testJunk main.cpp)
find_package(GTest REQUIRED)
target_link_libraries(testJunk GTest::gtest_main)
add_test(AllTestsInCJunk testJunk)