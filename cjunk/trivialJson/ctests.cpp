// catch main
#define CATCH_CONFIG_MAIN

#include "catch2/catch_all.hpp"

// Simple smoke test to ensure Catch2 is working
TEST_CASE("Catch2 smoke test", "[smoke]") {
    REQUIRE(1 + 1 == 2);
    REQUIRE(true);
}
