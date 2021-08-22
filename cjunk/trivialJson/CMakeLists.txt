
Include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)
FetchContent_MakeAvailable(Catch2)

#find_package(Catch2 REQUIRED)


list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_library(trivialJson json.cpp)
target_include_directories(trivialJson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(jsonTests tests/main.cpp)
target_link_libraries(jsonTests PRIVATE Catch2::Catch2WithMain trivialJson)
#catch_discover_tests(jsonTests)
