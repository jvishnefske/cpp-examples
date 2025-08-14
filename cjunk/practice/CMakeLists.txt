# only compile sendfile if sendfile.h is available
include(CheckIncludeFile)
check_include_file(sendfile.h SENDFILE)
if(SENDFILE)
    add_executable(sendFile sendFile.cc)
    add_test(sendFile sendFile)
endif(SENDFILE)

find_package(Threads REQUIRED)

# compile only the clean practice files as a library
file(GLOB practice_sources *.cc)
message(STATUS "practice_sources: ${practice_sources}")
add_library(practice ${practice_sources})

# Link threads for any files that use std::thread/async
target_link_libraries(practice PRIVATE Threads::Threads)

# Create individual executables for the main practice examples with main() functions
add_executable(functional_demo functional.cc)
add_test(functional_demo functional_demo)
target_link_libraries(functional_demo PRIVATE Threads::Threads)

add_executable(smallstring_demo smallString.cc)
add_test(smallstring_demo smallstring_demo)
target_link_libraries(smallstring_demo PRIVATE Threads::Threads)

add_executable(actor_demo actorReturningVoid.cc)
add_test(actor_demo actor_demo)
target_link_libraries(actor_demo PRIVATE Threads::Threads)
# Try to find OpenCV for the neural network demo
find_package(OpenCV QUIET)
if(OpenCV_FOUND)
    add_executable(imageNeuralNetwork ImageNeuralNetwork.cpp)
    add_test(imageNeuralNetwork imageNeuralNetwork)
    target_link_libraries(imageNeuralNetwork PRIVATE ${OpenCV_LIBS})
    target_include_directories(imageNeuralNetwork PRIVATE ${OpenCV_INCLUDE_DIRS})
    message(STATUS "OpenCV found, building imageNeuralNetwork")
else()
    message(STATUS "OpenCV not found, skipping imageNeuralNetwork")
endif()

add_executable(egl_test egl/egl_test.cpp)
add_test(egl_test egl_test)
