// http3_fuzzer.cpp
// LibFuzzer harness for HTTP/3 client
// Compile with: clang++ -g -fsanitize=fuzzer,address,undefined -std=c++17 http3_fuzzer.cpp

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>
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
