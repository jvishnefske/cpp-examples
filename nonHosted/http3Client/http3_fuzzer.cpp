// http3_fuzzer.cpp
// LibFuzzer harness for HTTP/3 client
// Compile with: clang++ -g -fsanitize=fuzzer,address,undefined -std=c++17 http3_fuzzer.cpp

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <string> // Required for std::string
#include <vector> // Required for std::vector in ConsumeBytes
#include "http3_client.hpp"

// Fuzzer for different components of the HTTP/3 client
namespace {

// Helper to consume bytes from fuzzer input
class FuzzedDataProvider {
private:
    const uint8_t* data_;
    size_t remaining_;

public:
    FuzzedDataProvider(const uint8_t* data, size_t size) noexcept
        : data_(data), remaining_(size) {}

    template<typename T>
    T ConsumeIntegral() noexcept {
        T result{};
        size_t num_bytes = std::min(remaining_, sizeof(T));
        if (num_bytes > 0) {
            std::memcpy(&result, data_, num_bytes);
            data_ += num_bytes;
            remaining_ -= num_bytes;
        }
        return result;
    }

    std::string ConsumeString(size_t max_length) noexcept {
        size_t length = ConsumeIntegral<uint8_t>() % (max_length + 1);
        length = std::min(length, remaining_);
        
        std::string result(reinterpret_cast<const char*>(data_), length);
        data_ += length;
        remaining_ -= length;
        
        return result;
    }

    std::vector<uint8_t> ConsumeBytes(size_t max_length) noexcept {
        size_t length = ConsumeIntegral<uint16_t>() % (max_length + 1);
        length = std::min(length, remaining_);
        
        std::vector<uint8_t> result(data_, data_ + length);
        data_ += length;
        remaining_ -= length;
        
        return result;
    }

    const uint8_t* data() const noexcept { return data_; }
    size_t remaining() const noexcept { return remaining_; }
};

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    using namespace http3;

    FuzzedDataProvider provider(Data, Size);

    if (provider.remaining() < 10) {
        // Need at least some bytes for basic operations
        return 0;
    }

    // Fuzz HttpHeader
    HttpHeader header;
    std::string name = provider.ConsumeString(63);
    std::string value = provider.ConsumeString(255);
    (void)header.set_header(std::string_view(name), std::string_view(value));

    // Fuzz JsonBuilder
    JsonBuilder json_builder;
    (void)json_builder.start_object();
    for (int i = 0; i < 5 && provider.remaining() > 0; ++i) {
        std::string json_key = provider.ConsumeString(30);
        std::string json_value = provider.ConsumeString(100);
        (void)json_builder.add_string_field(std::string_view(json_key), std::string_view(json_value), i == 4);
    }
    (void)json_builder.end_object();

    // Fuzz Http3Client connect
    Http3Client client = create_http3_client();
    std::string hostname = provider.ConsumeString(Config::MAX_HOSTNAME_LENGTH);
    std::uint16_t port = provider.ConsumeIntegral<uint16_t>();

    // We expect connect to fail or succeed in a controlled way,
    // actual network connection is mocked, so we test validation logic mostly.
    (void)client.connect(std::string_view(hostname), port);
    if (client.is_connected()) {
        // If connected (mocked), try to fuzz a POST request
        std::string path = provider.ConsumeString(Config::MAX_PATH_LENGTH);
        (void)client.post_json(std::string_view(path), json_builder);
        (void)client.disconnect();
    }

    // Fuzz Http3Frame payload setting
    Http3Frame frame;
    std::vector<uint8_t> frame_payload = provider.ConsumeBytes(Config::MAX_PACKET_SIZE);
    (void)frame.set_payload(frame_payload.data(), frame_payload.size());

    // Fuzz AltSvcInfo parsing
    AltSvcInfo alt_svc_info;
    std::string alt_svc_str = provider.ConsumeString(200);
    (void)alt_svc_info.parse_alt_svc(std::string_view(alt_svc_str));

    // Fuzz QpackProcessor encoding/decoding
    QpackProcessor qpack_processor;
    std::array<HttpHeader, 5> fuzz_headers;
    std::size_t header_count = 0;
    for (int i = 0; i < 5 && provider.remaining() > 0; ++i) {
        std::string header_name = provider.ConsumeString(63);
        std::string header_value = provider.ConsumeString(255);
        if (header_count < fuzz_headers.size() &&
            qpack_processor.add_dynamic_entry(HttpHeader{}) == Result::Success) { // Add to dynamic table for fuzzing
            (void)fuzz_headers[header_count++].set_header(std::string_view(header_name), std::string_view(header_value));
        }
    }

    std::array<uint8_t, Config::MAX_HEADER_SIZE> encoded_headers_buffer;
    size_t encoded_length = 0;
    (void)qpack_processor.encode_headers(fuzz_headers.data(), header_count,
                                         encoded_headers_buffer.data(), encoded_headers_buffer.size(), encoded_length);

    std::array<HttpHeader, 5> decoded_headers_buffer;
    size_t decoded_count = 0;
    (void)qpack_processor.decode_headers(encoded_headers_buffer.data(), encoded_length,
                                         decoded_headers_buffer.data(), decoded_headers_buffer.size(), decoded_count);
    
    return 0;
}
