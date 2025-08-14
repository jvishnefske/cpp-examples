option(ENABLE_BENCHMARK "compile benchmark application" ON)
project(trivialJson)
#option(ENABLE_BENCHMARK "compile benchmark application" OFF)

set(CMAKE_CXX_STANDARD 17)
     cmake_minimum_required(VERSION 3.16)

#find_package(Catch2 REQUIRED)


list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_library(trivialJson json.cpp)
target_include_directories(trivialJson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(trivialJson PRIVATE fmt::fmt)
set(libraries_to_link
        Boost::boost
        trivialJson
        )
# Tests removed due to MISRA violations
#if(MSVC and STATIC_ANALYSIS)
#target_compile_options(jsonTests PRIVATE /fsanitize=address)
#endif()
#catch_discover_tests(jsonTests TEST_SPEC serialize_float)
#ParseAndAddCatchTests(jsonTests)

set(ENABLE_BENCHMARK OFF)
if (ENABLE_BENCHMARK)
     add_executable(jsonBenchmark tests/benchmark.cpp)
     add_test(jsonBenchmark jsonBenchmark)
     target_link_libraries(jsonBenchmark PRIVATE Catch2::Catch2 ${libraries_to_link})
     # we should not need to do this due to the interface library
     message("rapidjson location ${RAPIDJSON_INCLUDE}")
     target_include_directories(jsonBenchmark PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/rapidjson/include)
endif ()

add_executable(ctests ctests.cpp)
add_test(ctests ctests)
#target_link_libraries(ctests Catch2::Catch2)
target_link_libraries(ctests PRIVATE Catch2WithMain)

# SimdJson demo - only if simdjson is available
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SIMDJSON QUIET simdjson)
    if(SIMDJSON_FOUND)
        add_executable(fromsimd fromsimd.cpp)
        target_link_libraries(fromsimd PRIVATE ${SIMDJSON_LIBRARIES})
        target_include_directories(fromsimd PRIVATE ${SIMDJSON_INCLUDE_DIRS})
        add_test(NAME fromsimd COMMAND fromsimd)
    else()
        message(STATUS "simdjson not found, skipping fromsimd executable")
    endif()
else()
    message(STATUS "PkgConfig not available, skipping fromsimd executable")
endif()
