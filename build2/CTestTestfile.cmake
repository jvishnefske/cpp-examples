# CMake generated Testfile for 
# Source directory: /home/vis75817/src/cpp-examples
# Build directory: /home/vis75817/src/cpp-examples/build2
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(raii "/home/vis75817/src/cpp-examples/build2/raii")
set_tests_properties(raii PROPERTIES  _BACKTRACE_TRIPLES "/home/vis75817/src/cpp-examples/CMakeLists.txt;37;add_test;/home/vis75817/src/cpp-examples/CMakeLists.txt;0;")
subdirs("_deps/catch2-build")
subdirs("cjunk")
