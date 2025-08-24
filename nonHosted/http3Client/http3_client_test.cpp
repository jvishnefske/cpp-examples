// http3_client_test.cpp
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "http3_client.hpp"

// Test framework macros for MISRA compliance
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "Test failed: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::cerr << "Test failed: expected " << static_cast<int>(expected) \
                     << " but got " << static_cast<int>(actual) \
                     << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

namespace http3_test {

// Test fixture for organized testing
class Http3TestSuite {
private:
    static constexpr bool verbose_ = false;
    
    static void log_test(const char* test_name) noexcept {
        if (verbose_) {
            std::cout << "Running: " << test_name << std::endl;
        }
    }

public:
    // Test Result enum functionality
    [[nodiscard]] static bool test_result_enum() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        // Test enum values
        TEST_ASSERT(static_cast<std::uint8_t>(Result::Success) == 0U);
        TEST_ASSERT(static_cast<std::uint8_t>(Result::InvalidParameter) == 1U);
        TEST_ASSERT(static_cast<std::uint8_t>(Result::BufferOverflow) == 3U);
        
        // Test comparisons
        Result r1 = Result::Success;
        Result r2 = Result::Success;
        TEST_ASSERT(r1 == r2);
        
        r2 = Result::InvalidParameter;
        TEST_ASSERT(r1 != r2);
        
        return true;
    }
    
    // Test HttpHeader functionality
    [[nodiscard]] static bool test_http_header() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        HttpHeader header;
        
        // Test empty header
        TEST_ASSERT(header.get_name().empty());
        TEST_ASSERT(header.get_value().empty());
        
        // Test setting valid header
        auto result = header.set_header("Content-Type", "application/json");
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(header.get_name() == "Content-Type");
        TEST_ASSERT(header.get_value() == "application/json");
        
        // Test buffer overflow protection
        std::string long_name(65, 'a'); // Declared here
        result = header.set_header(long_name, "value"); // Assign the return value to result
        TEST_ASSERT_EQ(Result::BufferOverflow, result);
        
        std::string long_value(257, 'b');  // Exceeds 256 char limit
        result = header.set_header("name", long_value); // Assign the return value to result
        TEST_ASSERT_EQ(Result::BufferOverflow, result);
        
        // Test edge cases
        result = header.set_header("", "");
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(header.get_name().empty());
        TEST_ASSERT(header.get_value().empty());
        
        // Test maximum valid lengths
        std::string max_name(63, 'x');
        std::string max_value(255, 'y');
        result = header.set_header(max_name, max_value);
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(header.get_name() == max_name);
        TEST_ASSERT(header.get_value() == max_value);
        
        return true;
    }
    
    // Test Http3Frame functionality
    [[nodiscard]] static bool test_http3_frame() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        Http3Frame frame;
        
        // Test default initialization
        TEST_ASSERT(frame.type == FrameType::DATA);
        TEST_ASSERT(frame.payload_length == 0U);
        
        // Test setting valid payload
        std::array<std::uint8_t, 100U> test_data{};
        for (std::size_t i = 0U; i < test_data.size(); ++i) {
            test_data[i] = static_cast<std::uint8_t>(i);
        }
        
        auto result = frame.set_payload(test_data.data(), test_data.size()); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(frame.payload_length == test_data.size());
        
        // Verify payload content
        for (std::size_t i = 0U; i < test_data.size(); ++i) {
            TEST_ASSERT(frame.payload[i] == test_data[i]);
        }
        
        // Test null pointer with non-zero length
        result = frame.set_payload(nullptr, 10U); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::InvalidParameter, result);
        
        // Test null pointer with zero length (should succeed)
        result = frame.set_payload(nullptr, 0U); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::Success, result);
        
        // Test buffer overflow
        std::array<std::uint8_t, Config::MAX_PACKET_SIZE + 1U> large_data;
        result = frame.set_payload(large_data.data(), large_data.size()); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::BufferOverflow, result);
        
        return true;
    }
    
    // Test QPACK Static Table
    [[nodiscard]] static bool test_qpack_static_table() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        QpackStaticTable table;
        
        // Test finding known entries
        auto index = table.find_entry(":method", "GET");
        TEST_ASSERT(index.has_value());
        TEST_ASSERT(*index == 1U);
        
        index = table.find_entry(":method", "POST");
        TEST_ASSERT(index.has_value());
        TEST_ASSERT(*index == 2U);
        
        index = table.find_entry(":path", "/");
        TEST_ASSERT(index.has_value());
        TEST_ASSERT(*index == 3U);
        
        // Test partial match (name only)
        index = table.find_entry(":status");
        TEST_ASSERT(index.has_value());
        
        // Test non-existent entry
        index = table.find_entry("non-existent-header");
        TEST_ASSERT(!index.has_value());
        
        // Test retrieving entries by index
        auto entry = table.get_entry(0U);
        TEST_ASSERT(entry.has_value());
        TEST_ASSERT(entry->get_name() == ":authority");
        
        entry = table.get_entry(1U);
        TEST_ASSERT(entry.has_value());
        TEST_ASSERT(entry->get_name() == ":method");
        TEST_ASSERT(entry->get_value() == "GET");
        
        // Test out of bounds index
        entry = table.get_entry(100U);
        TEST_ASSERT(!entry.has_value());
        
        return true;
    }
    
    // Test QPACK Dynamic Table
    [[nodiscard]] static bool test_qpack_dynamic_table() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        QpackDynamicTable table;
        
        // Test initial state
        TEST_ASSERT(table.size() == 0U);
        
        // Add entries
        HttpHeader header1;
        (void)header1.set_header("custom-header", "custom-value");
        auto result = table.add_entry(header1); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(table.size() == 1U);
        
        HttpHeader header2;
        (void)header2.set_header("another-header", "another-value");
        result = table.add_entry(header2); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(table.size() == 2U);
        
        // Retrieve entries
        auto entry = table.get_entry(0U);
        TEST_ASSERT(entry.has_value());
        TEST_ASSERT(entry->get_name() == "custom-header");
        TEST_ASSERT(entry->get_value() == "custom-value");
        
        entry = table.get_entry(1U);
        TEST_ASSERT(entry.has_value());
        TEST_ASSERT(entry->get_name() == "another-header");
        
        // Test out of bounds
        entry = table.get_entry(2U);
        TEST_ASSERT(!entry.has_value());
        
        // Test clear
        table.clear();
        TEST_ASSERT(table.size() == 0U);
        
        return true;
    }
    
    // Test QPACK Processor encoding/decoding
    [[nodiscard]] static bool test_qpack_processor() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        QpackProcessor processor;
        
        // Prepare headers to encode
        std::array<HttpHeader, 4U> headers;
        (void)headers[0].set_header(":method", "GET");
        (void)headers[1].set_header(":path", "/");
        (void)headers[2].set_header(":scheme", "https");
        (void)headers[3].set_header(":authority", "example.com");
        
        // Encode headers
        std::array<std::uint8_t, 1024U> encoded{};
        std::size_t encoded_length = 0U;
        
        auto result = processor.encode_headers(
            headers.data(), 4U,
            encoded.data(), encoded.size(), encoded_length
        );
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(encoded_length > 0U);
        
        // Decode headers
        std::array<HttpHeader, 10U> decoded_headers;
        std::size_t decoded_count = 0U;
        
        result = processor.decode_headers(
            encoded.data(), encoded_length,
            decoded_headers.data(), decoded_headers.size(), decoded_count
        );
        TEST_ASSERT_EQ(Result::Success, result);
        
        // Test with literal headers (not in static table)
        (void)headers[0].set_header("x-custom", "value1");
        (void)headers[1].set_header("x-another", "value2");
        
        result = processor.encode_headers(
            headers.data(), 2U,
            encoded.data(), encoded.size(), encoded_length
        );
        TEST_ASSERT_EQ(Result::Success, result);
        
        result = processor.decode_headers(
            encoded.data(), encoded_length,
            decoded_headers.data(), decoded_headers.size(), decoded_count
        );
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(decoded_count == 2U);
        TEST_ASSERT(decoded_headers[0].get_name() == "x-custom");
        TEST_ASSERT(decoded_headers[0].get_value() == "value1");
        
        // Test error cases
        result = processor.encode_headers(
            nullptr, 1U,
            encoded.data(), encoded.size(), encoded_length
        );
        TEST_ASSERT_EQ(Result::InvalidParameter, result);
        
        result = processor.decode_headers(
            nullptr, 10U,
            decoded_headers.data(), decoded_headers.size(), decoded_count
        );
        TEST_ASSERT_EQ(Result::InvalidParameter, result);
        
        return true;
    }
    
    // Test JsonBuilder
    [[nodiscard]] static bool test_json_builder() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        JsonBuilder json;
        
        // Build simple JSON object
        auto result = json.start_object(); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::Success, result);
        
        result = json.add_string_field("name", "test", false); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::Success, result);
        
        result = json.add_number_field("value", 42U, false); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::Success, result);
        
        result = json.add_string_field("status", "ok", true); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::Success, result);
        
        result = json.end_object(); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::Success, result);
        
        auto json_str = json.get_json();
        TEST_ASSERT(!json_str.empty());
        TEST_ASSERT(!json.has_error());
        
        // Expected: {"name":"test","value":42,"status":"ok"}
        TEST_ASSERT(json_str.find("\"name\":\"test\"") != std::string_view::npos);
        TEST_ASSERT(json_str.find("\"value\":42") != std::string_view::npos);
        TEST_ASSERT(json_str.find("\"status\":\"ok\"") != std::string_view::npos);
        
        // Test escaping
        json.reset();
        (void)json.start_object();
        (void)json.add_string_field("quote", "\"hello\"", true);
        (void)json.end_object();
        
        json_str = json.get_json();
        TEST_ASSERT(json_str.find("\\\"hello\\\"") != std::string_view::npos);
        
        // Test buffer overflow
        json.reset();
        (void)json.start_object();
        
        // Try to add a very long string that exceeds buffer
        std::string long_key(Config::MAX_JSON_SIZE / 2, 'a');
        std::string long_value(Config::MAX_JSON_SIZE / 2, 'b');
        
        result = json.add_string_field(long_key, long_value, true); // Result is used in TEST_ASSERT_EQ
        TEST_ASSERT_EQ(Result::BufferOverflow, result);
        TEST_ASSERT(json.has_error());
        TEST_ASSERT(json.get_json().empty());
        
        // Test number edge cases
        json.reset();
        (void)json.start_object();
        (void)json.add_number_field("zero", 0U, false);
        (void)json.add_number_field("max", UINT64_MAX, true);
        (void)json.end_object();
        
        json_str = json.get_json();
        TEST_ASSERT(json_str.find("\"zero\":0") != std::string_view::npos);
        
        return true;
    }
    
    // Test StreamManager
    [[nodiscard]] static bool test_stream_manager() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        StreamManager manager;
        
        // Test creating request streams (bidirectional)
        auto stream_id = manager.create_request_stream();
        TEST_ASSERT(stream_id == 0U);  // First client bidi stream
        
        stream_id = manager.create_request_stream();
        TEST_ASSERT(stream_id == 4U);  // Next client bidi stream (+4)
        
        stream_id = manager.create_request_stream();
        TEST_ASSERT(stream_id == 8U);  // Next client bidi stream (+4)
        
        // Test creating unidirectional streams
        stream_id = manager.create_unidirectional_stream(StreamType::CONTROL);
        TEST_ASSERT(stream_id == 2U);  // First client uni stream
        
        stream_id = manager.create_unidirectional_stream(StreamType::QPACK_ENCODER);
        TEST_ASSERT(stream_id == 6U);  // Next client uni stream (+4)
        
        // Test closing streams
        manager.close_stream(0U);
        manager.close_stream(4U);
        
        // After closing, we should be able to reuse slots
        // (implementation dependent behavior)
        
        return true;
    }
    
    // Test AltSvcInfo parsing
    [[nodiscard]] static bool test_alt_svc_parsing() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        AltSvcInfo alt_svc;
        
        // Test valid Alt-Svc header
        auto result = alt_svc.parse_alt_svc("h3=\":443\"; ma=86400");
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(alt_svc.valid);
        TEST_ASSERT(alt_svc.port == 443U);
        TEST_ASSERT(alt_svc.max_age == 86400U);
        
        // Test different port
        alt_svc = AltSvcInfo{};
        result = alt_svc.parse_alt_svc("h3=\":8443\"; ma=3600");
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(alt_svc.port == 8443U);
        TEST_ASSERT(alt_svc.max_age == 3600U);
        
        // Test missing h3
        alt_svc = AltSvcInfo{};
        result = alt_svc.parse_alt_svc("h2=\":443\"");
        TEST_ASSERT_EQ(Result::ProtocolError, result);
        TEST_ASSERT(!alt_svc.valid);
        
        // Test empty string
        result = alt_svc.parse_alt_svc("");
        TEST_ASSERT_EQ(Result::InvalidParameter, result);
        
        // Test partial header (no max-age)
        alt_svc = AltSvcInfo{};
        result = alt_svc.parse_alt_svc("h3=\":443\"");
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(alt_svc.port == 443U);
        TEST_ASSERT(alt_svc.max_age == 86400U);  // Default
        
        return true;
    }
    
    // Test hostname validation
    [[nodiscard]] static bool test_hostname_validation() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        auto client = create_http3_client();
        
        // Valid hostnames
        auto result = client.connect("example.com"); // Result is used in TEST_ASSERT
        TEST_ASSERT(result == Result::Success || result == Result::ProtocolError);
        (void)client.disconnect();
        
        result = client.connect("sub.domain.example.com"); // Result is used in TEST_ASSERT
        TEST_ASSERT(result == Result::Success || result == Result::ProtocolError);
        (void)client.disconnect();
        
        result = client.connect("example-with-dash.com"); // Result is used in TEST_ASSERT
        TEST_ASSERT(result == Result::Success || result == Result::ProtocolError);
        (void)client.disconnect();
        
        result = client.connect("192.168.1.1"); // Result is used in TEST_ASSERT
        TEST_ASSERT(result == Result::Success || result == Result::ProtocolError);
        (void)client.disconnect();
        
        // Invalid hostnames
        result = client.connect("example_with_underscore.com");
        TEST_ASSERT_EQ(Result::InvalidParameter, result);
        
        result = client.connect("example.com!");
        TEST_ASSERT_EQ(Result::InvalidParameter, result);
        
        result = client.connect("");
        TEST_ASSERT_EQ(Result::InvalidParameter, result);
        
        // Test maximum length
        std::string long_hostname(Config::MAX_HOSTNAME_LENGTH + 1, 'a');
        result = client.connect(long_hostname);
        TEST_ASSERT_EQ(Result::InvalidParameter, result);
        
        return true;
    }
    
    // Test frame type conversions
    [[nodiscard]] static bool test_frame_types() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        // Test frame type values match RFC 9114
        TEST_ASSERT(static_cast<std::uint64_t>(FrameType::DATA) == 0x00U);
        TEST_ASSERT(static_cast<std::uint64_t>(FrameType::HEADERS) == 0x01U);
        TEST_ASSERT(static_cast<std::uint64_t>(FrameType::CANCEL_PUSH) == 0x03U);
        TEST_ASSERT(static_cast<std::uint64_t>(FrameType::SETTINGS) == 0x04U);
        TEST_ASSERT(static_cast<std::uint64_t>(FrameType::PUSH_PROMISE) == 0x05U);
        TEST_ASSERT(static_cast<std::uint64_t>(FrameType::GOAWAY) == 0x07U);
        TEST_ASSERT(static_cast<std::uint64_t>(FrameType::MAX_PUSH_ID) == 0x0DU);
        
        // Test stream type values
        TEST_ASSERT(static_cast<std::uint64_t>(StreamType::CONTROL) == 0x00U);
        TEST_ASSERT(static_cast<std::uint64_t>(StreamType::PUSH) == 0x01U);
        TEST_ASSERT(static_cast<std::uint64_t>(StreamType::QPACK_ENCODER) == 0x02U);
        TEST_ASSERT(static_cast<std::uint64_t>(StreamType::QPACK_DECODER) == 0x03U);
        
        return true;
    }
    
    // Test HTTP status codes
    [[nodiscard]] static bool test_http_status() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        TEST_ASSERT(static_cast<std::uint16_t>(HttpStatus::OK) == 200U);
        TEST_ASSERT(static_cast<std::uint16_t>(HttpStatus::Created) == 201U);
        TEST_ASSERT(static_cast<std::uint16_t>(HttpStatus::BadRequest) == 400U);
        TEST_ASSERT(static_cast<std::uint16_t>(HttpStatus::NotFound) == 404U);
        TEST_ASSERT(static_cast<std::uint16_t>(HttpStatus::InternalServerError) == 500U);
        
        // Test status checking
        auto client = create_http3_client();
        
        // Since we can't actually connect, we test the logic
        TEST_ASSERT(client.get_last_status() == HttpStatus::Unknown);
        TEST_ASSERT(!client.last_request_successful());
        
        return true;
    }
    
    // Stress test for JsonBuilder
    [[nodiscard]] static bool test_json_builder_stress() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        JsonBuilder json;
        
        // Build complex nested-like structure
        (void)json.start_object();
        
        // Add many fields to test buffer management
        constexpr std::size_t num_fields = 50U;
        for (std::size_t i = 0U; i < num_fields; ++i) {
            std::array<char, 10U> key{};
            std::array<char, 10U> value{};
            
            // Create unique keys and values
            std::snprintf(key.data(), key.size(), "k%zu", i);
            std::snprintf(value.data(), value.size(), "v%zu", i);
            
            bool is_last = (i == num_fields - 1U);
            auto result = json.add_string_field(key.data(), value.data(), is_last);
            
            if (result != Result::Success) {
                // Should hit buffer limit at some point
                TEST_ASSERT(json.has_error());
                break;
            }
        }
        
        return true;
    }
    
    // Edge case testing for QPACK
    [[nodiscard]] static bool test_qpack_edge_cases() noexcept {
        log_test(__func__);
        
        using namespace http3;
        
        QpackProcessor processor;
        
        // Test empty header list
        std::array<std::uint8_t, 1024U> encoded{};
        std::size_t encoded_length = 0U;
        
        // Use a dummy header array for 0 header_count to avoid nullptr check
        std::array<HttpHeader, 1> dummy_headers;
        auto result = processor.encode_headers(
            dummy_headers.data(), 0U, // Pass a valid pointer, but 0 count
            encoded.data(), encoded.size(), encoded_length
        );
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(encoded_length == 2U);  // Just the required fields
        
        // Test decoding empty header list
        std::array<HttpHeader, 10U> headers;
        std::size_t header_count = 0U;
        
        result = processor.decode_headers(
            encoded.data(), encoded_length,
            headers.data(), headers.size(), header_count
        );
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(header_count == 0U);
        
        // Test maximum header name/value lengths
        HttpHeader long_header;
        std::string max_name(63, 'n');
        std::string max_value(255, 'v');
        (void)long_header.set_header(max_name, max_value);
        
        result = processor.encode_headers(
            &long_header, 1U,
            encoded.data(), encoded.size(), encoded_length
        );
        TEST_ASSERT_EQ(Result::Success, result);
        
        result = processor.decode_headers(
            encoded.data(), encoded_length,
            headers.data(), headers.size(), header_count
        );
        TEST_ASSERT_EQ(Result::Success, result);
        TEST_ASSERT(header_count == 1U);
        TEST_ASSERT(headers[0].get_name() == max_name);
        TEST_ASSERT(headers[0].get_value() == max_value);
        
        return true;
    }
};

// Main test runner
[[nodiscard]] bool run_all_tests() noexcept {
    std::cout << "Running HTTP/3 Client Unit Tests..." << std::endl;
    std::cout << "====================================" << std::endl;
    
    std::size_t passed = 0U;
    std::size_t failed = 0U;
    
    // Define all tests
    struct Test {
        const char* name;
        bool (*func)() noexcept;
    };
    
    constexpr std::array<Test, 14U> tests = {{
        {"Result Enum", Http3TestSuite::test_result_enum},
        {"HTTP Header", Http3TestSuite::test_http_header},
        {"HTTP/3 Frame", Http3TestSuite::test_http3_frame},
        {"QPACK Static Table", Http3TestSuite::test_qpack_static_table},
        {"QPACK Dynamic Table", Http3TestSuite::test_qpack_dynamic_table},
        {"QPACK Processor", Http3TestSuite::test_qpack_processor},
        {"JSON Builder", Http3TestSuite::test_json_builder},
        {"Stream Manager", Http3TestSuite::test_stream_manager},
        {"Alt-Svc Parsing", Http3TestSuite::test_alt_svc_parsing},
        {"Hostname Validation", Http3TestSuite::test_hostname_validation},
        {"Frame Types", Http3TestSuite::test_frame_types},
        {"HTTP Status", Http3TestSuite::test_http_status},
        {"JSON Builder Stress", Http3TestSuite::test_json_builder_stress},
        {"QPACK Edge Cases", Http3TestSuite::test_qpack_edge_cases}
    }};
    
    // Run all tests
    for (const auto& test : tests) {
        std::cout << std::setw(30) << std::left << test.name << ": ";
        
        if (test.func()) {
            std::cout << "✓ PASSED" << std::endl;
            ++passed;
        } else {
            std::cout << "✗ FAILED" << std::endl;
            ++failed;
        }
    }
    
    // Print summary
    std::cout << "====================================" << std::endl;
    std::cout << "Test Results: " << passed << " passed, " << failed << " failed" << std::endl;
    
    return failed == 0U;
}

} // namespace http3_test

// Entry point
int main() {
    bool success = http3_test::run_all_tests();
    return success ? 0 : 1;
}
