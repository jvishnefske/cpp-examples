Include(FetchContent)

set(CATCH_INSTALL_DOC OFF)
set(CATCH_INSTALL_EXTRAS OFF)
set(CATCH_BUILD_TESTING OFF)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.0.0-preview3
)
#FetchContent_MakeAvailable(Catch2)
# fetch like this since the default method tries to hit the server even when local content is avaliable.
FetchContent_GetProperties(Catch2)
if (NOT EXISTS ${Catch2_BINARY_DIR} OR NOT EXISTS ${Catch2_SOURCE_DIR})
    FetchContent_populate(Catch2)
endif ()
message("unpopulated catch dirs ${Catch2_SOURCE_DIR}, ${Catch2_BINARY_DIR}, pop: ${Catch2_POPULATED}")
add_subdirectory(${Catch2_SOURCE_DIR} ${Catch2_BINARY_DIR})


include(CTest)
#include(Catch2)
#include(ParseAndAddCatchTests)