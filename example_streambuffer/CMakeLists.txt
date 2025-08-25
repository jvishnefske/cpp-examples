
set(CMAKE_CXX_STANDARD 14)

# Always build testHello using the already available GoogleTest from FetchContent
add_executable(testHello testHello.cpp)
target_link_libraries(testHello PRIVATE GTest::gtest_main streaming)
add_test(NAME HelloTests COMMAND testHello)
set_tests_properties(HelloTests PROPERTIES TIMEOUT 10)


add_library(streaming library.cpp library.h )
# Test removed due to MISRA violations
add_executable(test_streaming gnustream.cpp)
add_test(test_streaming test_streaming)
target_link_libraries(test_streaming GTest::gtest_main streaming)
