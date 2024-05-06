cmake_minimum_required(VERSION 3.22)
project(cpp_examples)
option(ENABLE_BENCHMARK "compile benchmark application" OFF)
add_compile_options(-fsanitize=undefined -Werror=all -Werror=extra)
add_link_options(-fsanitize=undefined)
set(FETCHCONTENT_QUIET OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
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
include(cmake/dependencies.cmake)
declare_dependencies()
FetchContent_MakeAvailable(Boost Catch2 fmt)
enable_testing()
#include(cmake/pmm.cmake)

# consider grpc, but it takes a long time to compile which slows down ci.
# set( CMAKE_OSX_ARCHITECTURES arm64)
# if arm osx
message(STATUS "proc: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "gen: ${CMAKE_GENERATOR}")
if(CMAKE_SYSTEM_NAME MATCHES "Darwin" )
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        message(STATUS "arm64-osx")
        set(VCPKG_TARGET_TRIPLET "arm64-osx")
    else()
        message(STATUS "x86_64-osx")
        message(STATUS "architecture: ${CMAKE_OSX_ARCHITECTURES} ")
        set(VCPKG_TARGET_TRIPLET "x64-osx")
    endif()
else()
    message(STATUS "not osx")
endif()

#pmm(VCPKG REVISION 2021.05.12 REQUIRES asio catch2) #  asio nlohmann-json)

#The package asio:x86-windows provides CMake targets:
#find_package(asio CONFIG REQUIRED)
#target_link_libraries(main PRIVATE asio Boost::boost)
#The package catch2:x86-windows provides CMake targets:
#find_package(Catch2 CONFIG REQUIRED)
#target_link_libraries(main PRIVATE Catch2::Catch2)
#The package grpc:x86-windows provides CMake targets:
# find_package(gRPC)
# Note: 6 target(s) were omitted.
#target_link_libraries(main PRIVATE gRPC::gpr gRPC::upb gRPC::grpc gRPC::grpc++)
#find_package(nlohmann_json CONFIG REQUIRED)

set(RAPIDJSON_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rapidjson/include/")
if (ENABLE_BENCHMARK)
    add_library(rapidJson INTERFACE)
    target_include_directories(rapidJson INTERFACE ${RAPIDJSON_INCLUDE})
    add_library(rapidJson::rapidJson ALIAS rapidJson)
endif ()
set(libraries_to_link
        Boost::boost
        fmt::fmt
        trivialJson
        smallTrivialJson
        )

add_executable(raii raii.cc)
target_link_libraries(raii PRIVATE Catch2::Catch2WithMain)
add_test(NAME raii COMMAND $<TARGET_FILE:raii>)
add_subdirectory(cjunk)