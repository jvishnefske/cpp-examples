Include(FetchContent)

set(CATCH_INSTALL_DOC OFF)
set(CATCH_INSTALL_EXTRAS OFF)
set(CATCH_BUILD_TESTING OFF)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)
FetchContent_MakeAvailable(Catch2)
include(CTest)
#include(Catch2)
#include(ParseAndAddCatchTests)