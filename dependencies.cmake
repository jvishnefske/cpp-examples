include(FetchContent)
macro(default_dependency_options)
    #install dependencies with package manager if requested.
    # if this is not used, then the dependencies need to be installed manually so that find_package can suceed.
    #nlohmann options
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    # detect release
    set(CMAKE_OSX_ARCHITECTURES  arm64)
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        message(STATUS "disabling simdjson sanitization and extra checks for release performance.")
        set(SIMDJSON_SANITIZE off)
        set(SIMDJSON_SANITIZE_THREADS off)
        set(SIMDJSON_SANITIZE_UNDEFINED off)
        set(SIMDJSON_VERBOSE_LOGGING off)
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        message(STATUS "enabling simdjson sanitization and extra checks")
        set(CMAKE_OSX_ARCHITECTURES  arm64)
        set(SIMDJSON_SANITIZE on)
        set(SIMDJSON_SANITIZE_THREADS on)
        #only enable Ubsan on x86 and x64
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64")
            set(SIMDJSON_SANITIZE_UNDEFINED off)
        else()
            set(SIMDJSON_SANITIZE_UNDEFINED off)
            message(WARNING "this architecture ${CMAKE_SYSTEM_PROCESSOR} may not support sanitizing simdjson")
        endif()
        set(SIMDJSON_VERBOSE_LOGGING on)
    else()
        message(STATUS "could not detect build type ${CMAKE_BUILD_TYPE}")
    endif()
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        message(STATUS "osx detected")
        set(VCPKG_TRIPLET arm64-osx)
        #set(PACKAGES ${PACKAGES} mvfst)
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        # mvfst not supported on windows.
        #    set(PACKAGES ${PACKAGES} ms-quic)
        set(PACKAGES ${PACKAGES} mdnsresponder)
        message(STATUS "windows detected, selecting 64 bit")
        set(VCPKG_TRIPLET x64-windows)
        # set minimum windows api verson needed for asio on windows
        # 0xA00 = windows 10
        # confer https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
        add_compile_definitions(_WIN32_WINNT=0x0A00)
    else ()
        message(STATUS "osx not detected, assuming linux")
        set(PACKAGES ${PACKAGES} mvfst mdnsresponder)
        set(VCPKG_TRIPLET x64-linux)
    endif ()

    find_package(benchmark CONFIG)

endmacro()

function(vcpkg_add_catch)
            set(PACKAGES catch2)
            pmm(VERBOSE VCPKG REVISION ${VCPKG_VERSION} TRIPLET ${VCPKG_TRIPLET} REQUIRES ${PACKAGES})
endfunction()
            # if a user attempted to find_library, and it was NOTFOUND, we fetch, and make avaliable here.

macro(delclare_contentfetch_if_missing)
    set(gtest_hide_internal_symbols ON)
    FetchContent_Declare(GoogleTest URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
            URL_HASH MD5=e82199374acdfda3f425331028eb4e2a)
    message(STATUS "GoogleTest retrieved ${googletest_POPULATED} at ${googletest_SOURCE_DIR}")
    FetchContent_Declare(catch2 URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.1.0.tar.gz
                URL_HASH MD5=730ddfe3a57b5316f790b0347c217651)
    set(BENCHMARK_ENABLE_TESTING OFF )
    FetchContent_Declare(benchmark URL https://github.com/google/benchmark/archive/refs/tags/v1.6.2.tar.gz
            URL_HASH MD5=14d14849e075af116143a161bc3b927b)
    message(STATUS "FETCH_GOOGLE_BENCHMARK retrieved ${benchmark_POPULATED}")
    # headeronly version for now
    FethContent_Declare(
            Boost
            GIT_REPOSITORY https://github.com/jvishnefske/boost-headers
            GIT_TAG origin/main
            GIT_SHALLOW TRUE
    )
    #FetchContent_MakeAvailable(GoogleTest benchmark Boost catch2)
endmacro()
