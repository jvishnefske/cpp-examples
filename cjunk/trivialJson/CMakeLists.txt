project(trivialJson)
#option(ENABLE_BENCHMARK "compile benchmark application" OFF)

set(CMAKE_CXX_STANDARD 17)
     cmake_minimum_required(VERSION 3.16)

#find_package(Catch2 REQUIRED)


list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_library(smallTrivialJson SmallMemoryModel.cpp)
add_library(trivialJson json.cpp)
target_include_directories(trivialJson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(jsonTests tests/main.cpp)
target_link_libraries(jsonTests PRIVATE ${libraries_to_link} Catch2WithMain trivialJson)
#if(MSVC and STATIC_ANALYSIS)
#target_compile_options(jsonTests PRIVATE /fsanitize=address)
#endif()
#catch_discover_tests(jsonTests TEST_SPEC serialize_float)
#ParseAndAddCatchTests(jsonTests)
add_test(NAME jsonTests COMMAND $<TARGET_FILE:jsonTests>)
set(ENABLE_BENCHMARK OFF)
if (ENABLE_BENCHMARK)
     add_executable(jsonBenchmark tests/benchmark.cpp)
     target_link_libraries(jsonBenchmark PRIVATE Catch2::Catch2 ${libraries_to_link})
     # we should not need to do this due to the interface library
     message("rapidjson location ${RAPIDJSON_INCLUDE}")
     target_include_directories(jsonBenchmark PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/rapidjson/include)
endif ()

add_executable(ctests ctests.cpp)
#target_link_libraries(ctests Catch2::Catch2)
target_link_libraries(ctests PRIVATE Catch2WithMain)
