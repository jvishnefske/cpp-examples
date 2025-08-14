#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include <catch2/catch_all.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "json.hpp"
#include <random>
#include <array>

using namespace std::string_literals;
using JsonNode = Node;
//catch benchmark random number generator

TEST_CASE("benchmark random number generator", "[.]") {
    std::mt19937 rng{std::random_device{}()};
    BENCHMARK("random number generator") {
                                             (void) rng();
                                         };
//    BENCHMARK("JsonNode::parse")
//                        -> RangeMultiplier(
//    2)
//    ->Range(1, 1 << 10)
//            ->Unit(benchmark::kMillisecond);
}

TEST_CASE("serialize_float", "jsonTest") {
    JsonNode j(1.1);

    CHECK(24 >= sizeof(JsonNode)); // we don't want this to increase.
    REQUIRE(j.serialize().length() > 0);
    CHECK("1.100000" == j.serialize());
}

TEST_CASE("serialize_empty", "jsonTest") {
    JsonNode j;
    REQUIRE(j.serialize().length() > 0);
    CHECK("\"\"" == j.serialize());
}

TEST_CASE("serialize_string", "jsonTest") {
    JsonNode j("Cat");
    REQUIRE(j.serialize().length() > 0);
    CHECK("\"Cat\"" == j.serialize());
}
#if 0 //TODO ambiguous JsonNode constructor
TEST_CASE("serialize_int", "jsonTest") {
    JsonNode j(42L);
    REQUIRE(j.serialize().length() > 0);
    CHECK("42" == j.serialize());
}
#endif
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
    std::mt19937_64 e1(r());
    std::exponential_distribution<double> rand_e(10.0);
    for (int i = 0; i < 1000; i++) {
        auto initial = rand_e(e1);
        std::array<char, 1000> buffer{};
        snprintf(buffer.data(), buffer.size(), "%20.20f", initial);
        auto roundTrip = std::stod(std::string(buffer.data()));

        REQUIRE_THAT(initial, Catch::Matchers::WithinRel(roundTrip, 0.001));
    }
}

// Additional tests for 100% coverage
TEST_CASE("serialize_list", "jsonTest") {
    // Create a list to test ListPtr serialization
    auto list = std::make_shared<std::vector<JsonNode>>();
    list->emplace_back(JsonNode(42L));
    list->emplace_back(JsonNode(1.5));
    
    JsonNode j(list);
    REQUIRE(j.serialize().length() > 0);
    CHECK("[42,1.500000]" == j.serialize());
}

TEST_CASE("serialize_empty_list", "jsonTest") {
    // Test empty list case
    auto list = std::make_shared<std::vector<JsonNode>>();
    JsonNode j(list);
    REQUIRE(j.serialize().length() > 0);
    CHECK("[]" == j.serialize());
}

TEST_CASE("serialize_null_list", "jsonTest") {
    // Test nullptr ListPtr case (triggers "nullptr!!!" debug output)
    JsonNode::ListPtr nullList;
    JsonNode j(nullList);
    REQUIRE(j.serialize().length() > 0);
    CHECK("nullptr!!!" == j.serialize());
}

TEST_CASE("test_inequality_operator", "jsonTest") {
    JsonNode j1(42L);
    JsonNode j2(43L);
    JsonNode j3(42L);
    
    CHECK(j1 != j2);
    CHECK_FALSE(j1 != j3);
}

TEST_CASE("test_smallstring_serialization", "jsonTest") {
    // Test SmallString path in JsonVisitor
    JsonNode::SmallString smallStr{};
    std::copy_n("test", 4, smallStr.data());
    JsonNode j(smallStr);
    
    REQUIRE(j.serialize().length() > 0);
    // Should contain the quoted string
    CHECK(j.serialize().find("test") != std::string::npos);
}

TEST_CASE("test_char_constructor", "jsonTest") {
    // Test char* constructor (creates SmallString, max 8 chars)
    const char* shortString = "test123";
    JsonNode j(shortString);
    
    REQUIRE(j.serialize().length() > 0);
    std::string serialized = j.serialize();
    CHECK(serialized.find("test123") != std::string::npos);
}

TEST_CASE("test_integer_constructor", "jsonTest") {
    // Use int64_t to avoid ambiguity  
    JsonNode j_int(static_cast<int64_t>(42));
    
    REQUIRE(j_int.serialize().length() > 0);
    CHECK("42" == j_int.serialize());
}

TEST_CASE("test_ostream_operator", "jsonTest") {
    JsonNode j(static_cast<int64_t>(42));
    std::ostringstream oss;
    oss << j;
    CHECK("42" == oss.str());
}