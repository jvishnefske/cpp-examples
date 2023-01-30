set(practice_libraries Boost::boost)
add_executable(chainablePromise chainablePromise.cc)
# only compile sendfileif sendfile.h is avaliable
include(CheckIncludeFile)
check_include_file(sendfile.h SENDFILE)
if(SENDFILE)
    add_executable(sendFile sendFile.cc)
endif(SENDFILE)
add_executable(isatty isatty.cc)
add_executable(asyncStdin asyncStdin.cc)
find_library(PTHREAD pthread REQUIRED)
target_link_libraries(asyncStdin PRIVATE ${practice_libraries} ${PTHREAD})

# compile everything as a library
file(GLOB practice_sources *.cc)
message(STATUS "practice_sources: ${practice_sources}")
#add_library(practice ${practice_sources})
#target_link_libraries(practice ${practice_libraries})
#target_compile_features(practice PRIVATE cxx_std_20)
