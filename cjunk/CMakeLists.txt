cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(cjunk)


add_subdirectory(trivialJson)
add_subdirectory(practice)

# Optional subdirectories with external dependencies
find_package(OpenGL QUIET)
if(OpenGL_FOUND OR TARGET OpenGL::GL)
    add_subdirectory(glFrontEnd)
endif()

# Radio demo requires libv4l2 
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(V4L2 QUIET libv4l2)
    if(V4L2_FOUND)
        add_subdirectory(swradio)
    endif()
endif()

# Poco demo requires Poco
find_package(Poco QUIET COMPONENTS Net)
if(Poco_FOUND)
    add_subdirectory(pocoDemo)
endif()
