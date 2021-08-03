find_package(Poco REQUIRED Net JSON)
add_executable(PocoJunk main.cpp)
target_link_libraries(PocoJunk Poco::Net)
# demonstrait command parsing.
#add_executable(PocoMain PocoMain.cpp)