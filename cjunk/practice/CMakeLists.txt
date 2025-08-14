find_package(Threads REQUIRED)

# Consolidated test executable that includes all main function logic from practice files
add_executable(test_all_practice test_all_practice.cpp)
target_link_libraries(test_all_practice PRIVATE Catch2::Catch2WithMain Threads::Threads)

# Try to find OpenCV for the neural network parts
find_package(OpenCV QUIET)
if(OpenCV_FOUND)
    target_link_libraries(test_all_practice PRIVATE ${OpenCV_LIBS})
    target_include_directories(test_all_practice PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_compile_definitions(test_all_practice PRIVATE HAVE_OPENCV=1)
    message(STATUS "OpenCV found, enabling OpenCV tests in consolidated test")
else()
    message(STATUS "OpenCV not found, disabling OpenCV tests in consolidated test")
endif()

# Find Boost for websocket and async tests
find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(test_all_practice PRIVATE Boost::system)

# Register the test with CTest
add_test(NAME practice_tests COMMAND test_all_practice)

# Optional: still compile sendfile if sendfile.h is available (as standalone utility)
include(CheckIncludeFile)
check_include_file(sendfile.h SENDFILE)
if(SENDFILE)
    add_executable(sendFile sendFile.cc)
    add_test(sendFile sendFile)
endif(SENDFILE)
