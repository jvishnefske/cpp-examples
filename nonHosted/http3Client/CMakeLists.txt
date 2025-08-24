cmake_minimum_required(VERSION 3.1)
project(http3_client)
set(CMAKE_EXPORT_COMPILE_COMMANDS CACHED true BOOL "")
add_executable(http3_test http3_client_test.cpp)
add_test(http3_test http3_test)
add_executable(http3_fuzzer http3_fuzzer.cpp)
target_compile_options(http3_fuzzer PRIVATE
    -g
    -fsanitize=fuzzer,address,undefined
)
target_link_options(http3_fuzzer PRIVATE
    -fsanitize=fuzzer,address,undefined
)
add_compile_options(http3_test PRIVATE -fsanitize-coverage=trace-pc)
