#include <catch2/catch_all.hpp>
#include "json.hpp"

TEST_CASE("forst", "jsonTest"){
	CHECK(true);
	CHECK(false);
	CHECK(1==1);
	REQUIRE(1==0);
};


#if 0
BENCHMARK("serialization"){
	JsonObject o;
	o="hi"
	auto result = serialize(o);
	REQUIRE(o==result);
}
#endif