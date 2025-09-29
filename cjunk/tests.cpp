/**
 * Comprehensive tests for cjunk components
 * Organized by component/functionality with clear test naming
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Component headers
#include "trivialJson/json.hpp"
#include "ringBuffer.h"

// Boost iostreams for header-only stream functionality
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>

// ============================================================================
// BOOST IOSTREAMS TESTS (header-only functionality)
// ============================================================================

template<typename Container>
class container_source {
public:
    typedef typename Container::value_type  char_type;
    typedef boost::iostreams::source_tag    category;
    
    container_source(Container& container)
            : container_(container), pos_(0) { }
    
    std::streamsize read(char_type* s, std::streamsize n) {
        using namespace std;
        streamsize amt = static_cast<streamsize>(container_.size() - pos_);
        streamsize result = (min)(n, amt);
        if (result != 0) {
            std::copy(container_.begin() + pos_,
                     container_.begin() + pos_ + result,
                     s);
            pos_ += result;
            return result;
        } else {
            return -1; // EOF
        }
    }
    
    Container& container() { return container_; }
    
private:
    typedef typename Container::size_type   size_type;
    Container&  container_;
    size_type   pos_;
};

class BoostIostreamTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up common test data
    }
};

TEST_F(BoostIostreamTests, BasicStreamOperations) {
    std::ostringstream oss{};
    std::istringstream iss{"inputString"};
    
    // Test basic stream operations
    std::string test_data = "test";
    oss << test_data;
    ASSERT_EQ(oss.str(), test_data);
    
    // Test input stream
    std::string result;
    iss >> result;
    ASSERT_EQ(result, "inputString");
}

TEST_F(BoostIostreamTests, ContainerSourceStream) {
    std::string input = "test string with multiple words";
    std::string output;
    boost::iostreams::stream<container_source<std::string>> in(input);
    getline(in, output);
    ASSERT_EQ(input, output);
}

TEST_F(BoostIostreamTests, ContainerSourcePartialRead) {
    std::string input = "partial";
    boost::iostreams::stream<container_source<std::string>> in(input);
    
    char buffer[4];
    in.read(buffer, 4);
    std::string partial(buffer, 4);
    
    ASSERT_EQ(partial, "part");
    ASSERT_EQ(in.gcount(), 4);
}

// ============================================================================
// RING BUFFER TESTS (C-style macro-based FIFO)
// ============================================================================

// Declare test fifos using the macro system
DeclareFifo(int, 5)
DeclareFifo(char, 3)

class RingBufferTests : public ::testing::Test {
protected:
    void SetUp() override {
        intFifo_init(&int_fifo);
        charFifo_init(&char_fifo);
    }
    
    intFifo int_fifo;
    charFifo char_fifo;
};

TEST_F(RingBufferTests, InitialState) {
    ASSERT_TRUE(intFifo_isEmpty(&int_fifo));
    ASSERT_EQ(intFifo_size(&int_fifo), 0);
}

TEST_F(RingBufferTests, BasicPushPop) {
    int value = 42;
    int output;
    
    ASSERT_EQ(intFifo_push(&int_fifo, value), SUCCESS);
    ASSERT_FALSE(intFifo_isEmpty(&int_fifo));
    ASSERT_EQ(intFifo_size(&int_fifo), 1);
    
    ASSERT_EQ(intFifo_pop(&int_fifo, &output), SUCCESS);
    ASSERT_EQ(output, value);
    ASSERT_TRUE(intFifo_isEmpty(&int_fifo));
}

TEST_F(RingBufferTests, FillToCapacity) {
    // Fill buffer to capacity (N-1 for circular buffer)
    for (int i = 0; i < 4; ++i) {
        ASSERT_EQ(intFifo_push(&int_fifo, i * 10), SUCCESS);
    }
    
    ASSERT_EQ(intFifo_size(&int_fifo), 4);
    
    // Should fail to add one more (full)
    ASSERT_EQ(intFifo_push(&int_fifo, 999), FAILURE);
}

TEST_F(RingBufferTests, WrapAroundBehavior) {
    // Fill and empty several times to test wrap-around
    for (int cycle = 0; cycle < 3; ++cycle) {
        // Fill buffer
        for (int i = 0; i < 4; ++i) {
            ASSERT_EQ(intFifo_push(&int_fifo, cycle * 100 + i), SUCCESS);
        }
        
        // Empty buffer
        for (int i = 0; i < 4; ++i) {
            int output;
            ASSERT_EQ(intFifo_pop(&int_fifo, &output), SUCCESS);
            ASSERT_EQ(output, cycle * 100 + i);
        }
        
        ASSERT_TRUE(intFifo_isEmpty(&int_fifo));
    }
}

TEST_F(RingBufferTests, PopFromEmpty) {
    int output;
    ASSERT_EQ(intFifo_pop(&int_fifo, &output), FAILURE);
}

TEST_F(RingBufferTests, CharFifoBasic) {
    char input = 'A';
    char output;
    
    ASSERT_EQ(charFifo_push(&char_fifo, input), SUCCESS);
    ASSERT_EQ(charFifo_pop(&char_fifo, &output), SUCCESS);
    ASSERT_EQ(output, input);
}

// ============================================================================
// BYTE SWAP UTILITY TESTS
// ============================================================================

class ByteSwapTests : public ::testing::Test {};

TEST_F(ByteSwapTests, ByteSwap16) {
    uint16_t input = 0x1234;
    uint16_t expected = 0x3412;
    ASSERT_EQ(byteswap_16(input), expected);
}

TEST_F(ByteSwapTests, ByteSwap32) {
    uint32_t input = 0x12345678;
    uint32_t expected = 0x78563412;
    ASSERT_EQ(byteswap_32(input), expected);
}

TEST_F(ByteSwapTests, ByteSwapSymmetry) {
    uint16_t value16 = 0xABCD;
    ASSERT_EQ(byteswap_16(byteswap_16(value16)), value16);
    
    uint32_t value32 = 0xDEADBEEF;
    ASSERT_EQ(byteswap_32(byteswap_32(value32)), value32);
}

// ============================================================================
// JSON PARSER TESTS (trivialJson integration)
// ============================================================================

class JsonParserTests : public ::testing::Test {
protected:
    Json::Reader reader;
    Json::Value result;
};

TEST_F(JsonParserTests, BasicJsonParsing) {
    std::string json_str = R"({"key": "value", "number": 42})";
    
    bool success = reader.parse(json_str, result);
    ASSERT_TRUE(success) << "JSON parsing failed: " << reader.getFormattedErrorMessages();
    
    ASSERT_TRUE(result.is_object());
    ASSERT_TRUE(result["key"].is_string());
    ASSERT_EQ(result["key"].as_string(), "value");
    ASSERT_TRUE(result["number"].is_number());
    ASSERT_EQ(result["number"].as_int(), 42);
}

TEST_F(JsonParserTests, JsonArray) {
    std::string json_str = R"([1, 2, 3, "test"])";
    
    bool success = reader.parse(json_str, result);
    ASSERT_TRUE(success);
    
    ASSERT_TRUE(result.is_array());
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[static_cast<size_t>(0)].as_int(), 1);
    ASSERT_EQ(result[static_cast<size_t>(3)].as_string(), "test");
}

TEST_F(JsonParserTests, NestedJson) {
    std::string json_str = R"({
        "user": {
            "name": "John",
            "age": 30,
            "hobbies": ["reading", "coding"]
        },
        "active": true
    })";
    
    bool success = reader.parse(json_str, result);
    ASSERT_TRUE(success);
    
    ASSERT_TRUE(result["user"]["name"].is_string());
    ASSERT_EQ(result["user"]["name"].as_string(), "John");
    ASSERT_EQ(result["user"]["age"].as_int(), 30);
    ASSERT_TRUE(result["user"]["hobbies"].is_array());
    ASSERT_EQ(result["user"]["hobbies"][static_cast<size_t>(1)].as_string(), "coding");
    ASSERT_TRUE(result["active"].is_bool());
}

TEST_F(JsonParserTests, InvalidJson) {
    std::string invalid_json = R"({"key": value})";  // Missing quotes around value
    
    bool success = reader.parse(invalid_json, result);
    ASSERT_FALSE(success);
    
    std::string errors = reader.getFormattedErrorMessages();
    ASSERT_FALSE(errors.empty());
}

TEST_F(JsonParserTests, JsonBuilder) {
    JsonBuilder builder;
    builder.start_object()
           .add_string_field("name", "Alice")
           .add_int_field("id", 123)
           .add_bool_field("admin", false)
           .end_object();
    
    Node built = builder.build();
    std::string serialized = built.serialize();
    
    // Parse the built JSON to verify structure
    Json::Value verification;
    bool success = reader.parse(serialized, verification);
    ASSERT_TRUE(success);
    
    ASSERT_EQ(verification["name"].as_string(), "Alice");
    ASSERT_EQ(verification["id"].as_int(), 123);
    ASSERT_FALSE(verification["admin"].as_bool());
}

TEST_F(JsonParserTests, JsonBuilderArray) {
    JsonBuilder builder;
    builder.start_array()
           .add_string_value("first")
           .add_int_value(42)
           .add_bool_value(true)
           .end_array();
    
    Node built = builder.build();
    
    ASSERT_TRUE(built.is_array());
    ASSERT_EQ(built.size(), 3);
    ASSERT_EQ(built[static_cast<size_t>(0)].as_string(), "first");
    ASSERT_EQ(built[static_cast<size_t>(1)].as_int(), 42);
    ASSERT_TRUE(built[static_cast<size_t>(2)].as_bool());
}

// ============================================================================
// INTEGRATION TESTS
// ============================================================================

class IntegrationTests : public ::testing::Test {};

TEST_F(IntegrationTests, JsonWithRingBuffer) {
    // Test combining JSON parsing with ring buffer storage
    // Use the already declared intFifo
    intFifo json_fifo;
    intFifo_init(&json_fifo);
    
    // Parse JSON array
    std::string json_str = "[10, 20, 30, 40, 50]";
    Json::Value json_array;
    Json::Reader reader;
    
    ASSERT_TRUE(reader.parse(json_str, json_array));
    ASSERT_TRUE(json_array.is_array());
    
    // Store values in ring buffer (int FIFO has capacity 5, can hold 4 items)
    for (size_t i = 0; i < json_array.size() && i < 4; ++i) {  // Only push 4 items
        int value = json_array[i].as_int();
        ASSERT_EQ(intFifo_push(&json_fifo, value), SUCCESS);
    }
    
    ASSERT_EQ(intFifo_size(&json_fifo), 4);
    
    // Retrieve and verify values (only the first 4)
    for (int expected = 10; expected <= 40; expected += 10) {  // Only expect first 4 values
        int output;
        ASSERT_EQ(intFifo_pop(&json_fifo, &output), SUCCESS);
        ASSERT_EQ(output, expected);
    }
}

TEST_F(IntegrationTests, StreamWithJsonSerialization) {
    // Create JSON, serialize to stream, then parse back
    JsonBuilder builder;
    builder.start_object()
           .add_string_field("message", "Hello World")
           .add_int_field("count", 3)
           .end_object();
    
    Node original = builder.build();
    std::string json_str = original.serialize();
    
    // Use container_source to stream the JSON string
    boost::iostreams::stream<container_source<std::string>> json_stream(json_str);
    std::string streamed_json;
    std::getline(json_stream, streamed_json);
    
    // Parse the streamed JSON
    Json::Value parsed;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(streamed_json, parsed));
    
    ASSERT_EQ(parsed["message"].as_string(), "Hello World");
    ASSERT_EQ(parsed["count"].as_int(), 3);
}

// ============================================================================
// PERFORMANCE/STRESS TESTS
// ============================================================================

class PerformanceTests : public ::testing::Test {};

// Declare a separate FIFO for performance testing with larger size
typedef int perf_int;
DeclareFifo(perf_int, 1000)

TEST_F(PerformanceTests, RingBufferThroughput) {
    perf_intFifo perf_fifo;
    perf_intFifo_init(&perf_fifo);
    
    // Fill and empty buffer multiple times
    for (int cycle = 0; cycle < 10; ++cycle) {
        // Fill buffer
        for (int i = 0; i < 999; ++i) {  // Leave one space
            ASSERT_EQ(perf_intFifo_push(&perf_fifo, i), SUCCESS);
        }
        
        // Empty buffer
        for (int i = 0; i < 999; ++i) {
            int output;
            ASSERT_EQ(perf_intFifo_pop(&perf_fifo, &output), SUCCESS);
            ASSERT_EQ(output, i);
        }
    }
}

TEST_F(PerformanceTests, JsonParsingStress) {
    // Test parsing multiple JSON objects
    const int num_objects = 100;
    Json::Reader reader;
    
    for (int i = 0; i < num_objects; ++i) {
        std::ostringstream json_stream;
        json_stream << R"({"id": )" << i << R"(, "name": "user)" << i << R"("})";
        
        Json::Value result;
        bool success = reader.parse(json_stream.str(), result);
        ASSERT_TRUE(success);
        ASSERT_EQ(result["id"].as_int(), static_cast<int64_t>(i));
    }
}