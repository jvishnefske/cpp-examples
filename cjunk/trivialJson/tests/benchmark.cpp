//
// Created by vis75817 on 8/25/2021.
//
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "benchmark.h"
// target_include_directory seems broken on windows??
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <json.hpp>
#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

TEST_CASE("benchmarks", "[benchmark]") {
    SECTION("rapidjson") {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.StartObject();
        writer.Key("key");
        writer.String("value");
        writer.EndObject();
        std::string str = buffer.GetString();
        REQUIRE(str == "{\"key\":\"value\"}");
    }SECTION("json") {
        nlohmann::json j;
        j["key"] = "value";
        std::string str = j.dump();
        REQUIRE(str == "{\"key\":\"value\"}");
    }
    BENCHMARK("simple") {
//    StringBuffer s;
//    Writer<StringBuffer> w(s);
                            std::cout << "benchmark this " << std::endl;
                        }
//    BENCHMARK_ADVANCED("trivial_serialize")(Catch::Benchmark::Chronometer meter)
//       {
//           auto obj = Node(1,2,3,4);
//           meter.measure([]{
//               auto s = obj.serialize();
//           });
//       };
}

