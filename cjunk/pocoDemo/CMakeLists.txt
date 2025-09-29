find_package(Poco REQUIRED Net)

add_executable(PocoDemo poco_demo.cpp)
target_link_libraries(PocoDemo Poco::Net)
add_test(PocoDemo PocoDemo)
