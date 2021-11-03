option(ENABLE_BENCHMARK "compile benchmark application" ON)

set(CMAKE_CXX_STANDARD 17)


#find_package(Catch2 REQUIRED)


list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)


add_library(trivialJson json.cpp)
target_include_directories(trivialJson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(jsonTests tests/main.cpp) # tests/benchmark.cpp tests/benchmark.h)
target_link_libraries(jsonTests PRIVATE Catch2::Catch2WithMain trivialJson)
#if(MSVC and STATIC_ANALYSIS)
#target_compile_options(jsonTests PRIVATE /fsanitize=address)
#endif()
#catch_discover_tests(jsonTests)
add_test(NAME jsonTests COMMAND $<TARGET_FILE:jsonTests>)


#add_executable(jsonBenchmark tests/benchmark.cpp)
#target_link_libraries(jsonBenchmark Catch2::Catch2WithMain rapidJson::rapidJson trivialJson)
# we should not need to do this due to the interface library
message("rapidjson location ${RAPIDJSON_INCLUDE}")
#target_include_directories(jsonBenchmark PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/rapidjson/include)
