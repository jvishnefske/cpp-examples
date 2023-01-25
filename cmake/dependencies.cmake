include(FetchContent)
macro(declare_dependencies)
    set(gtest_hide_internal_symbols ON)
    FetchContent_Declare(
            GoogleTest
            URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
            URL_HASH SHA256=81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2)
    FetchContent_Declare(
            catch2
            URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.1.0.tar.gz
            URL_HASH SHA256=c252b2d9537e18046d8b82535069d2567f77043f8e644acf9a9fffc22ea6e6f7)
    set(BENCHMARK_ENABLE_TESTING OFF)
    FetchContent_Declare(
            benchmark
            URL https://github.com/google/benchmark/archive/refs/tags/v1.6.2.tar.gz
            URL_HASH SHA256=a9f77e6188c1cd4ebedfa7538bf5176d6acc72ead6f456919e5f464ef2f06158)
    FetchContent_Declare(
            Poco
            URL https://github.com/pocoproject/poco/archive/refs/tags/poco-1.12.2-release.tar.gz
            URL_HASH SHA256=30442ccb097a0074133f699213a59d6f8c77db5b2c98a7c1ad9c5eeb3a2b06f3)
    FetchContent_Declare(
            Boost
            GIT_REPOSITORY https://github.com/jvishnefske/boost-headers
            GIT_TAG origin/main
            GIT_SHALLOW TRUE
    )
    #FetchContent_MakeAvailable(GoogleTest benchmark catch2 Poco)
endmacro()
