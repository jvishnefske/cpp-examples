
set(CMAKE_CXX_STANDARD 17)


#find_package(Catch2 REQUIRED)


list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)


add_library(trivialJson json.cpp)
target_include_directories(trivialJson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(jsonTests tests/main.cpp)
target_link_libraries(jsonTests PRIVATE Catch2::Catch2WithMain trivialJson)
#if(MSVC and STATIC_ANALYSIS)
#target_compile_options(jsonTests PRIVATE /fsanitize=address)
#endif()
#catch_discover_tests(jsonTests)
add_test(NAME jsonTests COMMAND $<TARGET_FILE:jsonTests>)
