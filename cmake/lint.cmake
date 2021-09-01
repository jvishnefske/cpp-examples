
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif()
find_program(CPPLINT_EXE "cpplint")

if (CPPLINT_EXE)
    message(STATUS "found: " ${CPPLINT_EXE})
    set(CMAKE_CXX_CPPLINT "${CPPLINT_EXE}")
    #else()
    #   unused, so don't compain
    #   message(AUTHOR_WARNING "cpplint not found")
endif()
find_program(CPPCHECK_EXE NAMES "cppcheck")
if (CPPCHECK_EXE)
    message(STATUS "found: " ${CPPCHECK_EXE})
    set(CMAKE_CXX_CPPCHECK "${CPPCHECK_EXE};--enable=all;--suppress=missingIncludeSystem;--error-exitcode=1")
else()
    message(AUTHOR_WARNING "cppcheck not found")
endif()
find_program(VALGRIND_EXE NAMES "valgrind")
if (VALGRIND_EXE)
    message(STATUS "found: " ${VALGRIND_EXE})
else()
    message(AUTHOR_WARNING "valgrind not found")
endif()

find_program(PERF_EXE NAMES "perf")
if (PERF_EXE)
    message(STATUS "found: " ${PERF_EXE})
else()
    message(AUTHOR_WARNING "perf not found")
endif()
find_program (CLANG_TIDY_EXE NAMES "clang-tidy" PATHS /usr/local/opt/llvm/bin )
if (CLANG_TIDY_EXE)
    message(STATUS "found: ${CLANG_TIDY_EXE}")
    set(CLANG_TIDY_CHECKS "-*,modernize-*")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=${CLANG_TIDY_CHECKS};-header-filter='${CMAKE_SOURCE_DIR}/*'" CACHE STRING "" FORCE)
else()
    message(AUTHOR_WARNING "clang-tidy not found!")
    set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE) # delete it
endif()
