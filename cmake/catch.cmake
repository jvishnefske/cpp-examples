Include(FetchContent)

set(CATCH_INSTALL_DOC OFF)
set(CATCH_INSTALL_EXTRAS OFF)
set(CATCH_BUILD_TESTING OFF)
set(FETCHCONTENT_QUIET OFF)


FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.0.0-preview3
)
# not FetchContent_MakeAvailable(Catch2)
# fetch like this since the default method tries to hit the server even when local content is avaliable.
FetchContent_GetProperties(catch2)
if (NOT DEFINED catch2_BINARY_DIR OR NOT DEFINED catch2_SOURCE_DIR)
    FetchContent_populate(Catch2)
    message("unpopulated catch dirs ${catch2_SOURCE_DIR}, ${catch2_BINARY_DIR}, pop: ${catch2_POPULATED}")
else ()
    message("populated catch dirs ${catch2_SOURCE_DIR}, ${catch2_BINARY_DIR}, pop: ${catch2_POPULATED}")
endif ()
add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
include (${catch2_SOURCE_DIR}/extras/ParseAndAddCatchTests.cmake)

include(CTest)
#include(${catch2_BINARY_DIR}/Catch2)
#include(ParseAndAddCatchTests)