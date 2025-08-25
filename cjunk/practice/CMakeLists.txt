find_package(Threads REQUIRED)

# Consolidated test executable that includes all main function logic from practice files
add_executable(test_all_practice test_all_practice.cpp)
target_link_libraries(test_all_practice PRIVATE Catch2::Catch2WithMain Threads::Threads)

# Try to find OpenCV for the neural network parts
find_package(OpenCV QUIET)
if(OpenCV_FOUND)
    target_link_libraries(test_all_practice PRIVATE ${OpenCV_LIBS})
    target_include_directories(test_all_practice PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_compile_definitions(test_all_practice PRIVATE HAVE_OPENCV=1)
    message(STATUS "OpenCV found, enabling OpenCV tests in consolidated test")
else()
    message(STATUS "OpenCV not found, disabling OpenCV tests in consolidated test")
endif()

# Find Boost for websocket and async tests
#find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(test_all_practice PRIVATE Boost::boost)

# Register the test with CTest
add_test(NAME practice_tests COMMAND test_all_practice)

# Optional: still compile sendfile if sendfile.h is available (as standalone utility)
include(CheckIncludeFile)
check_include_file(sendfile.h SENDFILE)
if(SENDFILE)
    add_executable(sendFile sendFile.cc)
    add_test(sendFile sendFile)
endif(SENDFILE)

# Add individual practice executables for coverage
set(PRACTICE_FILES
    smallString.cc
    financialAnalysis.cc 
    functional.cc
    asyncio.cc
    circularqueue.cc
    extendedKalanFilterBroken.cc
    geoParticleFilter.cc
    kalmanFilter.cc
    perceptron.cc
    unified_websocket.cpp
)

foreach(PRACTICE_FILE ${PRACTICE_FILES})
    get_filename_component(TARGET_NAME ${PRACTICE_FILE} NAME_WE)
    set(EXECUTABLE_NAME "practice_${TARGET_NAME}")
    
    add_executable(${EXECUTABLE_NAME} ${PRACTICE_FILE})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE Boost::boost Threads::Threads)
    
    # Add as test that just runs the executable
    add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})
    set_tests_properties(${EXECUTABLE_NAME} PROPERTIES TIMEOUT 30)
endforeach()

# Special handling for files that need additional dependencies
if(OpenCV_FOUND)
    add_executable(practice_ImageNeuralNetwork ImageNeuralNetwork.cpp)
    target_link_libraries(practice_ImageNeuralNetwork PRIVATE ${OpenCV_LIBS} Boost::boost)
    target_include_directories(practice_ImageNeuralNetwork PRIVATE ${OpenCV_INCLUDE_DIRS})
    add_test(NAME practice_ImageNeuralNetwork COMMAND practice_ImageNeuralNetwork)
    set_tests_properties(practice_ImageNeuralNetwork PROPERTIES TIMEOUT 30)
endif()

# UDP server examples
add_executable(practice_UDPServer2 UDPServer2.cc)
target_link_libraries(practice_UDPServer2 PRIVATE Boost::boost Threads::Threads)
add_test(NAME practice_UDPServer2 COMMAND practice_UDPServer2)
set_tests_properties(practice_UDPServer2 PROPERTIES TIMEOUT 10)

add_executable(practice_UDPSocketServer UDPSocketServer.cc) 
target_link_libraries(practice_UDPSocketServer PRIVATE Boost::boost Threads::Threads)
add_test(NAME practice_UDPSocketServer COMMAND practice_UDPSocketServer)
set_tests_properties(practice_UDPSocketServer PROPERTIES TIMEOUT 10)

add_executable(practice_antColonyOptimization antColonyOptimization.cc)
target_link_libraries(practice_antColonyOptimization PRIVATE Boost::boost Threads::Threads)
add_test(NAME practice_antColonyOptimization COMMAND practice_antColonyOptimization)
set_tests_properties(practice_antColonyOptimization PROPERTIES TIMEOUT 30)

# EGL test in subdirectory
add_executable(practice_egl_test egl/egl_test.cpp)
target_link_libraries(practice_egl_test PRIVATE Boost::boost Threads::Threads)
add_test(NAME practice_egl_test COMMAND practice_egl_test)
set_tests_properties(practice_egl_test PROPERTIES TIMEOUT 10)
