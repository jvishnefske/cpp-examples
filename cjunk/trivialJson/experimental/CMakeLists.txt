cmake_minimum_required(VERSION 3.16)

# Only build experimental code when benchmarks are enabled
if(ENABLE_BENCHMARK)
    # Add fromsimd executable
    add_executable(experimental_fromsimd fromsimd.cpp)
    target_link_libraries(experimental_fromsimd PRIVATE trivialJson fmt::fmt)
    add_test(NAME experimental_fromsimd COMMAND experimental_fromsimd)
    set_tests_properties(experimental_fromsimd PROPERTIES TIMEOUT 10)

    # Add tests subdirectory
    add_subdirectory(tests)
else()
    message(STATUS "Skipping experimental code - enable with -DENABLE_BENCHMARK=ON")
endif()