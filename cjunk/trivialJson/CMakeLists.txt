option(ENABLE_BENCHMARK "compile benchmark application" ON)

set(CMAKE_CXX_STANDARD 17)


#find_package(Catch2 REQUIRED)


list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)


add_library(trivialJson json.cpp)
target_include_directories(trivialJson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set(libraries_to_link
        asio::asio
        gRPC::gpr gRPC::upb gRPC::grpc gRPC::grpc++
        nlohmann_json::nlohmann_json)
add_executable(jsonTests tests/main.cpp tests/benchmark.h)
target_link_libraries(jsonTests PRIVATE ${libraries_to_link})
#if(MSVC and STATIC_ANALYSIS)
#target_compile_options(jsonTests PRIVATE /fsanitize=address)
#endif()
#catch_discover_tests(jsonTests)
add_test(NAME jsonTests COMMAND $<TARGET_FILE:jsonTests>)

add_executable(jsonBenchmark tests/benchmark.cpp)
target_link_libraries(jsonBenchmark PRIVATE Catch2::Catch2 ${libraries_to_link})
# we should not need to do this due to the interface library
message("rapidjson location ${RAPIDJSON_INCLUDE}")
target_include_directories(jsonBenchmark PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/rapidjson/include)


add_executable(ctests ctests.cpp)
#target_link_libraries(ctests Catch2::Catch2)
target_link_libraries(ctests PRIVATE ${libraries_to_link})
