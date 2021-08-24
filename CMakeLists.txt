cmake_minimum_required(VERSION 3.12)
project(cpp_examples)
# TODO: add to disable analysis
option(STATIC_ANALYSIS OFF)
if (MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        add_compile_options(/W4)
    endif()
    add_compile_options(/WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()
set(CMAKE_CXX_STANDARD 14)

if(STATIC_ANALYSIS)
    include(cmake/lint.cmake)
endif()
add_executable(raii raii.cc)
add_subdirectory(cjunk)