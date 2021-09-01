find_package(OpenGL)
add_executable(glFrontEnd Window.cpp)
target_link_libraries(glFrontEnd OpenGL::GL)