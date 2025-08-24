cmake_minimum_required(VERSION 3.20)
project(http3_client)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
add_executable(http3_test http3_client_test.cpp)
add_test(http3_test http3_test)
add_executable(http3_fuzzer http3_fuzzer.cpp)
# Check if fuzzer sanitizer is supported (requires clang with fuzzing support)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "6.0")
    target_compile_options(http3_fuzzer PRIVATE
        -g
        -fsanitize=fuzzer,address,undefined
    )
    target_link_options(http3_fuzzer PRIVATE
        -fsanitize=fuzzer,address,undefined
    )
else()
    # Skip fuzzer build for compilers that don't support it
    set_target_properties(http3_fuzzer PROPERTIES EXCLUDE_FROM_ALL TRUE)
    message(STATUS "Skipping http3_fuzzer build - requires Clang with fuzzing support")
endif()
add_compile_options(http3_test PRIVATE -fsanitize-coverage=trace-pc)
