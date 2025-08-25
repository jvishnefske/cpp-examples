cmake_minimum_required(VERSION 3.13)
project(radio_demo)

# query pkgconfig for videodev
find_package(PkgConfig REQUIRED)
pkg_check_modules(RADIO_LIB2 REQUIRED libv4l2)
if(NOT RADIO_LIB2_FOUND)
    message(FATAL_ERROR "missing radio library libv4l2")
endif()

# check if the header exists in /usr/include
find_file(VIDEO_INCLUDE_FILE NAMES videodev2.h HINTS "/usr/include/linux")
if(VIDEO_INCLUDE_FILE)
    # Build the standalone radio demo 
    add_executable(radio_demo radio_demo.cpp)
    target_link_libraries(radio_demo PRIVATE ${RADIO_LIB2_LIBRARIES})
    target_include_directories(radio_demo PRIVATE ${RADIO_LIB2_INCLUDE_DIRS})
    target_compile_options(radio_demo PRIVATE ${RADIO_LIB2_CFLAGS_OTHER})
    target_compile_features(radio_demo PRIVATE cxx_std_17)
    add_test(NAME radio_demo COMMAND radio_demo)
    set_tests_properties(radio_demo PROPERTIES TIMEOUT 10)
    
    # Add main.cpp as a separate target that uses the full radio.hpp interface
    add_executable(swradio_main main.cpp radio.cpp)
    target_link_libraries(swradio_main PRIVATE ${RADIO_LIB2_LIBRARIES})
    target_include_directories(swradio_main PRIVATE ${RADIO_LIB2_INCLUDE_DIRS})
    target_compile_options(swradio_main PRIVATE ${RADIO_LIB2_CFLAGS_OTHER})
    target_compile_features(swradio_main PRIVATE cxx_std_17)
    add_test(NAME swradio_main COMMAND swradio_main)
    set_tests_properties(swradio_main PROPERTIES TIMEOUT 10)
    
else()
    message(FATAL_ERROR "videodev2.h not found, skipping radio_demo")
endif()
