include(FetchContent)
macro(declare_dependencies)
    set(gtest_hide_internal_symbols ON)
    FetchContent_Declare(
            GoogleTest
            URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
            URL_HASH SHA256=81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_Declare(
            Catch2
            URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.5.4.tar.gz
            URL_HASH SHA256=b7754b711242c167d8f60b890695347f90a1ebc95949a045385114165d606dbb
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(BENCHMARK_ENABLE_TESTING OFF)
    FetchContent_Declare(
            benchmark
            URL https://github.com/google/benchmark/archive/refs/tags/v1.6.2.tar.gz
            URL_HASH SHA256=a9f77e6188c1cd4ebedfa7538bf5176d6acc72ead6f456919e5f464ef2f06158
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_Declare(
            Poco
            URL https://github.com/pocoproject/poco/archive/refs/tags/poco-1.12.2-release.tar.gz
            URL_HASH SHA256=30442ccb097a0074133f699213a59d6f8c77db5b2c98a7c1ad9c5eeb3a2b06f3
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_Declare(
            Boost
            GIT_REPOSITORY https://github.com/jvishnefske/boost-headers
            GIT_TAG v1.81.0
#            GIT_SHALLOW TRUE
    )
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt/
            GIT_TAG 10.x
            GIT_SHALLOW TRUE
    )
    #FetchContent_MakeAvailable(GoogleTest benchmark catch2 Poco fmt)
endmacro()
