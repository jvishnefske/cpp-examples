//#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "json.hpp"
#include <random>
#include <array>

using namespace std::string_literals;
using JsonNode = Node;

TEST_CASE("serialize_float", "jsonTest") {
    JsonNode j(1.1);
    // sizeof is 16 on visual studio
    STATIC_REQUIRE(24 >= sizeof(JsonNode)); // we don't want this to increase.
    REQUIRE(j.serialize().length() > 0);
    CHECK("1.100000" == j.serialize());
}

TEST_CASE("serialize_empty", "jsonTest"){
    JsonNode j;
    REQUIRE(j.serialize().length() > 0);
    CHECK("\"\"" == j.serialize());
}

TEST_CASE("serialize_string", "jsonTest"){
    JsonNode j ("Cat");
    REQUIRE(j.serialize().length() > 0);
    CHECK("\"Cat\"" == j.serialize());
}

TEST_CASE("serialize_int", "jsonTest"){
    JsonNode j ( 42 );
    REQUIRE(j.serialize().length() > 0);
    CHECK("42" == j.serialize());
}

//TEST_CASE("serialize_list", "jsonTest") {
//    JsonNode j(1, 2);
//    REQUIRE(j.serialize().length() > 0);
//    CHECK("[1,2]" == j.serialize());
//}

// this tests tries to convert a sequence of floats to string, and back.
// not a trivial task to get exact it turns out.
// a more typical expectation
// is to get within epsilon, or one count on the least significatnt decimal
TEST_CASE("round_trip_conversion", "[!mayfail]") {

    std::random_device r;
    std::default_random_engine e1(r());
    std::exponential_distribution<double> rand_e(10.0);
    for (int i = 0; i < 1000; i = i + 1) {
        auto initial = rand_e(e1);
        std::array<char, 1000> buffer{};
        snprintf(buffer.data(), buffer.size(), "%20.20f", initial);
        auto roundTrip = std::stod(std::string(buffer.data()));
        REQUIRE(initial == roundTrip);
    }
} 