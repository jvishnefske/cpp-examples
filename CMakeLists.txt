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
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif ()
set(CMAKE_CXX_STANDARD 14)

if (STATIC_ANALYSIS)
    include(cmake/lint.cmake)
endif ()
enable_testing()
include(cmake/pmm.cmake)

pmm(VCPKG REVISION 2021.05.12 REQUIRES catch2 grpc asio nlohmann-json)

#The package asio:x86-windows provides CMake targets:
find_package(asio CONFIG REQUIRED)
#target_link_libraries(main PRIVATE asio asio::asio)
#The package catch2:x86-windows provides CMake targets:
find_package(Catch2 CONFIG REQUIRED)
#target_link_libraries(main PRIVATE Catch2::Catch2)
#The package grpc:x86-windows provides CMake targets:
find_package(gRPC CONFIG REQUIRED)
# Note: 6 target(s) were omitted.
#target_link_libraries(main PRIVATE gRPC::gpr gRPC::upb gRPC::grpc gRPC::grpc++)
find_package(nlohmann_json CONFIG REQUIRED)

set(RAPIDJSON_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rapidjson/include/")
if (ENABLE_BENCHMARK)
    add_library(rapidJson INTERFACE)
    target_include_directories(rapidJson INTERFACE ${RAPIDJSON_INCLUDE})
    add_library(rapidJson::rapidJson ALIAS rapidJson)
endif ()

add_executable(raii raii.cc)
target_link_libraries(raii PRIVATE Catch2::Catch2)
add_test(NAME raii COMMAND $<TARGET_FILE:raii>)
add_subdirectory(cjunk)
add_subdirectory(external/Catch2)
#find_package(Catch2 REQUIRED)