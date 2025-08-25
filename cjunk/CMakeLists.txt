cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(cjunk)


add_subdirectory(trivialJson)
add_subdirectory(practice)

# Optional subdirectories with external dependencies
find_package(OpenGL QUIET)
if(OpenGL_FOUND OR TARGET OpenGL::GL)
    add_subdirectory(glFrontEnd)
endif()

# Radio demo requires libv4l2 
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(V4L2 QUIET libv4l2)
    if(V4L2_FOUND)
        add_subdirectory(swradio)
    endif()
endif()

# Poco demo requires Poco
find_package(Poco QUIET COMPONENTS Net)
if(Poco_FOUND)
    add_subdirectory(pocoDemo)
endif()

# Add standalone files for coverage

# main.cpp contains GTest tests but no main function
add_executable(cjunk_main main.cpp)
target_link_libraries(cjunk_main PRIVATE GTest::gtest_main)
add_test(NAME cjunk_main COMMAND cjunk_main)
set_tests_properties(cjunk_main PROPERTIES TIMEOUT 10)

# parser.cpp now uses trivialJson instead of external JsonCpp
add_library(cjunk_parser_lib STATIC parser.cpp)
target_include_directories(cjunk_parser_lib PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cjunk_parser_lib PRIVATE trivialJson)

# Create a test executable for parser
add_executable(cjunk_parser_test)
target_sources(cjunk_parser_test PRIVATE parser.cpp)
target_compile_definitions(cjunk_parser_test PRIVATE STANDALONE_TEST=1)
target_link_libraries(cjunk_parser_test PRIVATE trivialJson)
add_test(NAME cjunk_parser_test COMMAND cjunk_parser_test)
set_tests_properties(cjunk_parser_test PROPERTIES TIMEOUT 5)

add_executable(cjunk_tests tests.cpp)
target_include_directories(cjunk_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cjunk_tests PRIVATE GTest::gtest_main GTest::gmock trivialJson)
add_test(NAME cjunk_tests COMMAND cjunk_tests)

add_executable(cjunk_ringBuffer ringBuffer.cpp)
add_test(NAME cjunk_ringBuffer COMMAND cjunk_ringBuffer)
set_tests_properties(cjunk_ringBuffer PROPERTIES TIMEOUT 5)
