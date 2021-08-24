//#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "json.hpp"


using namespace std::string_literals;
using JsonNode = Node;
TEST_CASE("serialize_float", "jsonTest"){
    JsonNode j(1.1);
    CHECK(41 > sizeof(JsonNode)); // we don't want this to increase.
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

TEST_CASE("serialize_list", "jsonTest"){
    JsonNode j(1,2);
    REQUIRE(j.serialize().length() > 0);
    CHECK("[1,2]" == j.serialize());
}

#if 0
BENCHMARK("serialization"){
	JsonObject o;
	o="hi"
	auto result = serialize(o);
	REQUIRE(o==result);
}
#endif