#find_package(Poco REQUIRED Net JSON)
add_executable(PocoDemo main.cpp)
target_link_libraries(PocoJunk Poco::Net)
add_test(PocoJunk PocoDemo