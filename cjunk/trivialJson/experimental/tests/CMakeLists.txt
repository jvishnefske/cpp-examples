cmake_minimum_required(VERSION 3.16)

# Experimental test executables
add_executable(experimental_benchmark benchmark.cpp)
target_link_libraries(experimental_benchmark PRIVATE 
    trivialJson 
    fmt::fmt 
    Catch2::Catch2WithMain
)
target_compile_definitions(experimental_benchmark PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)
add_test(NAME experimental_benchmark COMMAND experimental_benchmark)
set_tests_properties(experimental_benchmark PROPERTIES TIMEOUT 30)

add_executable(experimental_main main.cpp)
target_link_libraries(experimental_main PRIVATE 
    trivialJson 
    fmt::fmt 
    Catch2::Catch2WithMain
)  
target_compile_definitions(experimental_main PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING)
add_test(NAME experimental_main COMMAND experimental_main)
set_tests_properties(experimental_main PROPERTIES TIMEOUT 30)