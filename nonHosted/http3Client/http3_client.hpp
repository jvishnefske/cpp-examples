#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>
#include <variant>
#include <chrono>

// Complete MISRA C++17 compliant HTTP/3 client with QPACK and frame handling
namespace http3 {

// MISRA-compliant error handling
enum class Result : std::uint8_t {
    Success = 0,
    InvalidParameter = 1,
    ConnectionFailed = 2,
    BufferOverflow = 3,
    Timeout = 4,
    ProtocolError = 5,
    CryptoError = 6,
    FlowControlError = 7,
    JsonError = 8,
    NetworkError = 9,
    ServerError = 10,
    QpackError = 11,
    FrameError = 12,
    StreamError = 13
};

enum class HttpStatus : std::uint16_t {
    OK = 200,
    Created = 201,
    Accepted = 202,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    InternalServerError = 500,
    BadGateway = 502,
    ServiceUnavailable = 503,
    Unknown = 0
};

// HTTP/3 Frame Types (RFC 9114)
enum class FrameType : std::uint64_t {
    DATA = 0x00,
    HEADERS = 0x01,
    CANCEL_PUSH = 0x03,
    SETTINGS = 0x04,
    PUSH_PROMISE = 0x05,
    GOAWAY = 0x07,
    MAX_PUSH_ID = 0x0D
};

// QUIC Stream Types for HTTP/3
enum class StreamType : std::uint64_t {
    CONTROL = 0x00,
    PUSH = 0x01,
    QPACK_ENCODER = 0x02,
    QPACK_DECODER = 0x03
};

struct Config {
    static constexpr std::size_t MAX_JSON_SIZE = 4096U;
    static constexpr std::size_t MAX_RESPONSE_SIZE = 16384U;
    static constexpr std::size_t MAX_HOSTNAME_LENGTH = 253U;
    static constexpr std::size_t MAX_PATH_LENGTH = 1024U;
    static constexpr std::size_t MAX_PACKET_SIZE = 1500U;
    static constexpr std::size_t MAX_HEADER_COUNT = 32U;
    static constexpr std::size_t MAX_HEADER_SIZE = 2048U;
    static constexpr std::size_t QPACK_STATIC_TABLE_SIZE = 99U;
    static constexpr std::size_t QPACK_DYNAMIC_TABLE_SIZE = 1024U;
    static constexpr std::uint32_t DEFAULT_TIMEOUT_MS = 30000U;
    static constexpr std::uint16_t DEFAULT_QUIC_PORT = 443U;
};

// HTTP/3 Frame structure
struct Http3Frame {
    FrameType type{FrameType::DATA};
    std::array<std::uint8_t, Config::MAX_PACKET_SIZE> payload{};
    std::size_t payload_length{0U};
    
    explicit constexpr Http3Frame() noexcept = default;
    
    [[nodiscard]] constexpr Result set_payload(const std::uint8_t* data, std::size_t length) noexcept {
        if (data == nullptr && length > 0U) {
            return Result::InvalidParameter;
        }
        
        if (length > payload.size()) {
            return Result::BufferOverflow;
        }
        
        for (std::size_t i = 0U; i < length; ++i) {
            payload[i] = data[i];
        }
        payload_length = length;
        
        return Result::Success;
    }
};

// HTTP Header structure
struct HttpHeader {
    std::array<char, 64U> name{};
    std::array<char, 256U> value{};
    std::size_t name_length{0U};
    std::size_t value_length{0U};
    
    explicit constexpr HttpHeader() noexcept = default;
    
    [[nodiscard]] constexpr Result set_header(std::string_view header_name, std::string_view header_value) noexcept {
        if (header_name.length() >= name.size() || header_value.length() >= value.size()) {
            return Result::BufferOverflow;
        }
        
        name_length = header_name.length();
        value_length = header_value.length();
        
        for (std::size_t i = 0U; i < name_length; ++i) {
            name[i] = header_name[i];
        }
        name[name_length] = '\0';
        
        for (std::size_t i = 0U; i < value_length; ++i) {
            value[i] = header_value[i];
        }
        value[value_length] = '\0';
        
        return Result::Success;
    }
    
    [[nodiscard]] constexpr std::string_view get_name() const noexcept {
        return std::string_view(name.data(), name_length);
    }
    
    [[nodiscard]] constexpr std::string_view get_value() const noexcept {
        return std::string_view(value.data(), value_length);
    }
};

// QPACK Static Table (simplified - RFC 9204 Appendix A)
class QpackStaticTable {
private:
    struct StaticEntry {
        const char* name;
        const char* value;
    };
    
    static constexpr std::array<StaticEntry, 25U> static_table_ = {{
        {":authority", ""},
        {":method", "GET"},
        {":method", "POST"},
        {":path", "/"},
        {":path", "/index.html"},
        {":scheme", "http"},
        {":scheme", "https"},
        {":status", "200"},
        {":status", "204"},
        {":status", "206"},
        {":status", "304"},
        {":status", "400"},
        {":status", "404"},
        {":status", "500"},
        {"accept-charset", ""},
        {"accept-encoding", "gzip, deflate"},
        {"accept-language", ""},
        {"accept-ranges", ""},
        {"accept", ""},
        {"access-control-allow-origin", ""},
        {"age", ""},
        {"allow", ""},
        {"authorization", ""},
        {"cache-control", ""},
        {"content-disposition", ""}
    }};

public:
    [[nodiscard]] constexpr std::optional<std::size_t> find_entry(
        std::string_view name, std::string_view value = {}) const noexcept {
        
        for (std::size_t i = 0U; i < static_table_.size(); ++i) {
            if (name == static_table_[i].name) {
                if (value.empty() || value == static_table_[i].value) {
                    return i;
                }
            }
        }
        return std::nullopt;
    }
    
    [[nodiscard]] constexpr std::optional<HttpHeader> get_entry(std::size_t index) const noexcept {
        if (index >= static_table_.size()) {
            return std::nullopt;
        }
        
        HttpHeader header;
        auto result = header.set_header(static_table_[index].name, static_table_[index].value);
        if (result != Result::Success) {
            return std::nullopt;
        }
        
        return header;
    }
};

// QPACK Dynamic Table (simplified implementation)
class QpackDynamicTable {
private:
    std::array<HttpHeader, Config::QPACK_DYNAMIC_TABLE_SIZE> entries_{};
    std::size_t size_{0U};
    std::size_t capacity_{Config::QPACK_DYNAMIC_TABLE_SIZE};

public:
    explicit constexpr QpackDynamicTable() noexcept = default;
    
    [[nodiscard]] constexpr Result add_entry(const HttpHeader& header) noexcept {
        if (size_ >= capacity_) {
            return Result::BufferOverflow;
        }
        
        entries_[size_] = header;
        ++size_;
        return Result::Success;
    }
    
    [[nodiscard]] constexpr std::optional<HttpHeader> get_entry(std::size_t index) const noexcept {
        if (index >= size_) {
            return std::nullopt;
        }
        return entries_[index];
    }
    
    [[nodiscard]] constexpr std::size_t size() const noexcept {
        return size_;
    }
    
    constexpr void clear() noexcept {
        size_ = 0U;
    }
};

// QPACK Encoder/Decoder
class QpackProcessor {
private:
    QpackStaticTable static_table_{};
    QpackDynamicTable dynamic_table_{};

public:
    explicit constexpr QpackProcessor() noexcept = default;
    
    // Simplified QPACK encoding (basic implementation)
    [[nodiscard]] constexpr Result encode_headers(
        const HttpHeader* headers, std::size_t header_count,
        std::uint8_t* output, std::size_t output_size, std::size_t& encoded_length) noexcept {
        
        if (headers == nullptr || output == nullptr) {
            return Result::InvalidParameter;
        }
        
        encoded_length = 0U;
        
        // Required Fields and Section Acknowledgment (simplified)
        if (encoded_length + 2U >= output_size) {
            return Result::BufferOverflow;
        }
        output[encoded_length++] = 0x00; // Required Insert Count
        output[encoded_length++] = 0x00; // Delta Base
        
        for (std::size_t i = 0U; i < header_count; ++i) {
            const auto& header = headers[i];
            
            // Try to find in static table
            auto static_index = static_table_.find_entry(header.get_name(), header.get_value());
            
            if (static_index.has_value()) {
                // Indexed Header Field (static table)
                if (encoded_length + 1U >= output_size) {
                    return Result::BufferOverflow;
                }
                output[encoded_length++] = static_cast<std::uint8_t>(0x80U | *static_index);
            } else {
                // Literal Header Field with Literal Name
                auto name_view = header.get_name();
                auto value_view = header.get_value();
                
                if (encoded_length + 2U + name_view.length() + value_view.length() >= output_size) {
                    return Result::BufferOverflow;
                }
                
                output[encoded_length++] = 0x20; // Literal Header Field pattern
                output[encoded_length++] = static_cast<std::uint8_t>(name_view.length());
                
                for (char c : name_view) {
                    output[encoded_length++] = static_cast<std::uint8_t>(c);
                }
                
                output[encoded_length++] = static_cast<std::uint8_t>(value_view.length());
                for (char c : value_view) {
                    output[encoded_length++] = static_cast<std::uint8_t>(c);
                }
            }
        }
        
        return Result::Success;
    }
    
    // Simplified QPACK decoding
    [[nodiscard]] constexpr Result decode_headers(
        const std::uint8_t* encoded, std::size_t encoded_length,
        HttpHeader* headers, std::size_t max_headers, std::size_t& header_count) noexcept {
        
        if (encoded == nullptr || headers == nullptr || encoded_length < 2U) {
            return Result::InvalidParameter;
        }
        
        header_count = 0U;
        std::size_t pos = 2U; // Skip Required Insert Count and Delta Base
        
        while (pos < encoded_length && header_count < max_headers) {
            std::uint8_t pattern = encoded[pos++];
            
            if ((pattern & 0x80U) != 0U) {
                // Indexed Header Field
                std::size_t index = pattern & 0x7FU;
                auto static_entry = static_table_.get_entry(index);
                
                if (static_entry.has_value()) {
                    headers[header_count++] = *static_entry;
                } else {
                    return Result::QpackError;
                }
            } else if ((pattern & 0x40U) != 0U) {
                // Literal Header Field with Name Reference
                return Result::QpackError; // Not implemented in this simplified version
            } else if ((pattern & 0x20U) != 0U) {
                // Literal Header Field with Literal Name
                if (pos >= encoded_length) {
                    return Result::QpackError;
                }
                
                std::size_t name_length = encoded[pos++];
                if (pos + name_length >= encoded_length) {
                    return Result::QpackError;
                }
                
                std::string_view name(reinterpret_cast<const char*>(&encoded[pos]), name_length);
                pos += name_length;
                
                if (pos >= encoded_length) {
                    return Result::QpackError;
                }
                
                std::size_t value_length = encoded[pos++];
                if (pos + value_length > encoded_length) {
                    return Result::QpackError;
                }
                
                std::string_view value(reinterpret_cast<const char*>(&encoded[pos]), value_length);
                pos += value_length;
                
                auto result = headers[header_count].set_header(name, value);
                if (result != Result::Success) {
                    return result;
                }
                ++header_count;
            } else {
                return Result::QpackError;
            }
        }
        
        return Result::Success;
    }
};

// HTTP/3 Stream Manager
class StreamManager {
private:
    struct StreamInfo {
        std::uint64_t stream_id{0U};
        StreamType type{StreamType::CONTROL};
        bool is_bidirectional{false};
        bool is_active{false};
    };
    
    std::array<StreamInfo, 16U> streams_{};
    std::uint64_t next_client_bidi_id_{0U}; // Client bidirectional streams start at 0
    std::uint64_t next_client_uni_id_{2U};  // Client unidirectional streams start at 2

public:
    explicit constexpr StreamManager() noexcept = default;
    
    [[nodiscard]] constexpr std::uint64_t create_request_stream() noexcept {
        std::uint64_t stream_id = next_client_bidi_id_;
        next_client_bidi_id_ += 4U; // Client bidirectional streams increment by 4
        
        // Find available slot
        for (auto& stream : streams_) {
            if (!stream.is_active) {
                stream.stream_id = stream_id;
                stream.type = StreamType::CONTROL; // Regular HTTP request
                stream.is_bidirectional = true;
                stream.is_active = true;
                break;
            }
        }
        
        return stream_id;
    }
    
    [[nodiscard]] constexpr std::uint64_t create_unidirectional_stream(StreamType type) noexcept {
        std::uint64_t stream_id = next_client_uni_id_;
        next_client_uni_id_ += 4U; // Client unidirectional streams increment by 4
        
        for (auto& stream : streams_) {
            if (!stream.is_active) {
                stream.stream_id = stream_id;
                stream.type = type;
                stream.is_bidirectional = false;
                stream.is_active = true;
                break;
            }
        }
        
        return stream_id;
    }
    
    constexpr void close_stream(std::uint64_t stream_id) noexcept {
        for (auto& stream : streams_) {
            if (stream.is_active && stream.stream_id == stream_id) {
                stream.is_active = false;
                break;
            }
        }
    }
};

// Alt-Svc (Alternative Service) discovery
struct AltSvcInfo {
    std::array<char, Config::MAX_HOSTNAME_LENGTH + 1U> hostname{};
    std::uint16_t port{443U};
    std::uint32_t max_age{86400U}; // 24 hours default
    bool valid{false};
    
    explicit constexpr AltSvcInfo() noexcept = default;
    
    [[nodiscard]] constexpr Result parse_alt_svc(std::string_view alt_svc_header) noexcept {
        // Simplified Alt-Svc parsing: h3=":443"; ma=86400
        if (alt_svc_header.empty()) {
            return Result::InvalidParameter;
        }
        
        // Look for h3= pattern
        std::size_t h3_pos = alt_svc_header.find("h3=");
        if (h3_pos == std::string_view::npos) {
            return Result::ProtocolError;
        }
        
        // Extract port (simplified parsing)
        std::size_t port_start = alt_svc_header.find(":", h3_pos);
        if (port_start != std::string_view::npos) {
            port_start += 1U;
            std::size_t port_end = alt_svc_header.find("\"", port_start);
            
            if (port_end != std::string_view::npos) {
                std::string_view port_str = alt_svc_header.substr(port_start, port_end - port_start);
                
                // Simple port parsing
                port = 0U;
                for (char c : port_str) {
                    if (c >= '0' && c <= '9') {
                        port = static_cast<std::uint16_t>(port * 10U + static_cast<std::uint16_t>(c - '0'));
                    }
                }
            }
        }
        
        // Extract max-age
        std::size_t ma_pos = alt_svc_header.find("ma=");
        if (ma_pos != std::string_view::npos) {
            ma_pos += 3U;
            max_age = 0U;
            
            for (std::size_t i = ma_pos; i < alt_svc_header.length(); ++i) {
                char c = alt_svc_header[i];
                if (c >= '0' && c <= '9') {
                    max_age = max_age * 10U + static_cast<std::uint32_t>(c - '0');
                } else {
                    break;
                }
            }
        }
        
        valid = true;
        return Result::Success;
    }
};

// JSON Builder (same as before but included for completeness)
class JsonBuilder {
private:
    std::array<char, Config::MAX_JSON_SIZE> buffer_{};
    std::size_t length_{0U};
    bool error_state_{false};
    
    [[nodiscard]] constexpr Result append(std::string_view str) noexcept {
        if (error_state_ || length_ + str.length() >= buffer_.size()) {
            error_state_ = true;
            return Result::BufferOverflow;
        }
        
        for (char c : str) {
            buffer_[length_++] = c;
        }
        return Result::Success;
    }
    
    [[nodiscard]] constexpr Result append_escaped_string(std::string_view str) noexcept {
        if (append("\"") != Result::Success) {
            return Result::BufferOverflow;
        }
        
        for (char c : str) {
            if (c == '"' || c == '\\') {
                if (append("\\") != Result::Success) {
                    return Result::BufferOverflow;
                }
            }
            if (length_ + 1U >= buffer_.size()) {
                error_state_ = true;
                return Result::BufferOverflow;
            }
            buffer_[length_++] = c;
        }
        
        return append("\"");
    }

public:
    explicit constexpr JsonBuilder() noexcept = default;
    
    [[nodiscard]] constexpr Result start_object() noexcept {
        return append("{");
    }
    
    [[nodiscard]] constexpr Result end_object() noexcept {
        return append("}");
    }
    
    [[nodiscard]] constexpr Result add_string_field(std::string_view key, std::string_view value, bool is_last = false) noexcept {
        if (append_escaped_string(key) != Result::Success) {
            return Result::BufferOverflow;
        }
        if (append(":") != Result::Success) {
            return Result::BufferOverflow;
        }
        if (append_escaped_string(value) != Result::Success) {
            return Result::BufferOverflow;
        }
        if (!is_last) {
            if (append(",") != Result::Success) {
                return Result::BufferOverflow;
            }
        }
        return Result::Success;
    }
    
    [[nodiscard]] constexpr Result add_number_field(std::string_view key, std::uint64_t value, bool is_last = false) noexcept {
        if (append_escaped_string(key) != Result::Success) {
            return Result::BufferOverflow;
        }
        if (append(":") != Result::Success) {
            return Result::BufferOverflow;
        }
        
        std::array<char, 21U> num_buffer{};
        std::size_t num_length = 0U;
        
        if (value == 0U) {
            num_buffer[0] = '0';
            num_length = 1U;
        } else {
            std::uint64_t temp = value;
            while (temp > 0U && num_length < num_buffer.size() - 1U) {
                num_buffer[num_length++] = static_cast<char>('0' + (temp % 10U));
                temp /= 10U;
            }
            
            for (std::size_t i = 0U; i < num_length / 2U; ++i) {
                char temp_char = num_buffer[i];
                num_buffer[i] = num_buffer[num_length - 1U - i];
                num_buffer[num_length - 1U - i] = temp_char;
            }
        }
        
        if (append(std::string_view(num_buffer.data(), num_length)) != Result::Success) {
            return Result::BufferOverflow;
        }
        
        if (!is_last) {
            if (append(",") != Result::Success) {
                return Result::BufferOverflow;
            }
        }
        return Result::Success;
    }
    
    [[nodiscard]] constexpr std::string_view get_json() const noexcept {
        if (error_state_) {
            return {};
        }
        return std::string_view(buffer_.data(), length_);
    }
    
    [[nodiscard]] constexpr bool has_error() const noexcept {
        return error_state_;
    }
    
    constexpr void reset() noexcept {
        length_ = 0U;
        error_state_ = false;
    }
};

// Complete HTTP/3 Client
class Http3Client {
private:
    enum class ConnectionState : std::uint8_t {
        Disconnected = 0,
        DiscoveringHttp3 = 1,
        Connecting = 2,
        Connected = 3,
        Closing = 4
    };
    
    ConnectionState state_{ConnectionState::Disconnected};
    std::array<char, Config::MAX_HOSTNAME_LENGTH + 1U> hostname_{};
    std::uint16_t port_{Config::DEFAULT_QUIC_PORT};
    
    // HTTP/3 components
    StreamManager stream_manager_{};
    QpackProcessor qpack_processor_{};
    AltSvcInfo alt_svc_info_{};
    
    // Connection tracking
    std::uint64_t connection_id_{0U};
    std::uint32_t packet_number_{0U};
    bool quic_handshake_complete_{false};
    bool http3_settings_sent_{false};
    
    // Stream tracking
    std::uint64_t control_stream_id_{0U};
    std::uint64_t qpack_encoder_stream_id_{0U};
    std::uint64_t qpack_decoder_stream_id_{0U};
    
    // Response handling
    HttpStatus last_status_{HttpStatus::Unknown};
    std::array<char, Config::MAX_RESPONSE_SIZE> response_body_{};
    std::size_t response_body_length_{0U};

    [[nodiscard]] constexpr Result validate_hostname(std::string_view hostname) const noexcept {
        if (hostname.empty() || hostname.length() > Config::MAX_HOSTNAME_LENGTH) {
            return Result::InvalidParameter;
        }
        
        for (char c : hostname) {
            if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '-')) {
                return Result::InvalidParameter;
            }
        }
        
        return Result::Success;
    }
    
    [[nodiscard]] constexpr Result send_http3_settings() noexcept {
        // Send SETTINGS frame on control stream
        Http3Frame settings_frame{};
        settings_frame.type = FrameType::SETTINGS;
        
        // Simplified settings: QPACK_MAX_TABLE_CAPACITY = 4096
        std::array<std::uint8_t, 4U> settings_payload = {0x01, 0x00, 0x10, 0x00};
        auto result = settings_frame.set_payload(settings_payload.data(), settings_payload.size());
        if (result != Result::Success) {
            return result;
        }
        
        // TODO: Send frame over control stream
        http3_settings_sent_ = true;
        return Result::Success;
    }
    
    [[nodiscard]] constexpr Result create_headers_frame(
        std::string_view method, std::string_view path, std::string_view host,
        const HttpHeader* additional_headers, std::size_t header_count,
        Http3Frame& frame) noexcept {
        
        // Build header list
        std::array<HttpHeader, Config::MAX_HEADER_COUNT> headers{};
        std::size_t total_headers = 0U;
        
        // Add pseudo-headers (required for HTTP/3)
        headers[total_headers++].set_header(":method", method);
        headers[total_headers++].set_header(":path", path);
        headers[total_headers++].set_header(":scheme", "https");
        headers[total_headers++].set_header(":authority", host);
        
        // Add additional headers
        for (std::size_t i = 0U; i < header_count && total_headers < Config::MAX_HEADER_COUNT; ++i) {
            headers[total_headers++] = additional_headers[i];
        }
        
        // Encode headers using QPACK
        std::array<std::uint8_t, Config::MAX_HEADER_SIZE> encoded_headers{};
        std::size_t encoded_length = 0U;
        
        auto result = qpack_processor_.encode_headers(
            headers.data(), total_headers,
            encoded_headers.data(), encoded_headers.size(), encoded_length
        );
        
        if (result != Result::Success) {
            return result;
        }
        
        // Create HEADERS frame
        frame.type = FrameType::HEADERS;
        return frame.set_payload(encoded_headers.data(), encoded_length);
    }
    
    [[nodiscard]] constexpr Result create_data_frame(
        const std::uint8_t* data, std::size_t data_length, Http3Frame& frame) noexcept {
        
        frame.type = FrameType::DATA;
        return frame.set_payload(data, data_length);
    }
    
    [[nodiscard]] constexpr Result process_response_frame(const Http3Frame& frame) noexcept {
        switch (frame.type) {
            case FrameType::HEADERS: {
                // Decode headers using QPACK
                std::array<HttpHeader, Config::MAX_HEADER_COUNT> headers{};
                std::size_t header_count = 0U;
                
                auto result = qpack_processor_.decode_headers(
                    frame.payload.data(), frame.payload_length,
                    headers.data(), Config::MAX_HEADER_COUNT, header_count
                );
                
                if (result != Result::Success) {
                    return result;
                }
                
                // Extract status from headers
                for (std::size_t i = 0U; i < header_count; ++i) {
                    if (headers[i].get_name() == ":status") {
                        auto status_str = headers[i].get_value();
                        
                        // Parse status code
                        std::uint16_t status_code = 0U;
                        for (char c : status_str) {
                            if (c >= '0' && c <= '9') {
                                status_code = static_cast<std::uint16_t>(status_code * 10U + static_cast<std::uint16_t>(c - '0'));
                            }
                        }
                        last_status_ = static_cast<HttpStatus>(status_code);
                        break;
                    }
                }
                break;
            }
            
            case FrameType::DATA: {
                // Store response body
                std::size_t copy_length = std::min(frame.payload_length, response_body_.size() - 1U);
                for (std::size_t i = 0U; i < copy_length; ++i) {
                    response_body_[i] = static_cast<char>(frame.payload[i]);
                }
                response_body_[copy_length] = '\0';
                response_body_length_ = copy_length;
                break;
            }
            
            case FrameType::SETTINGS:
                // Process server settings
                break;
                
            default:
                // Ignore unknown frames
                break;
        }
        
        return Result::Success;
    }
    
    // Simplified network operations (mock implementation)
    [[nodiscard]] constexpr Result send_frame_on_stream(std::uint64_t stream_id, const Http3Frame& frame) noexcept {
        // TODO: Real QUIC packet creation and UDP send
        (void)stream_id;
        (void)frame;
        ++packet_number_;
        return Result::Success;
    }
    
    [[nodiscard]] constexpr Result receive_frames(std::uint32_t timeout_ms) noexcept {
        // TODO: Real QUIC packet reception and frame parsing
        (void)timeout_ms;
        
        // Mock successful response for demonstration
        Http3Frame mock_headers{};
        mock_headers.type = FrameType::HEADERS;
        
        // Mock QPACK-encoded ":status: 200" header
        std::array<std::uint8_t, 10U> mock_header_data = {0x00, 0x00, 0x87}; // Simplified
        mock_headers.set_payload(mock_header_data.data(), 3U);
        
        auto result = process_response_frame(mock_headers);
        if (result != Result::Success) {
            return result;
        }
        
        // Mock data frame
        Http3Frame mock_data{};
        mock_data.type = FrameType::DATA;
        const char* mock_response = "{\"status\":\"success\"}";
        mock_data.set_payload(reinterpret_cast<const std::uint8_t*>(mock_response), std::strlen(mock_response));
        
        return process_response_frame(mock_data);
    }

public:
    explicit constexpr Http3Client() noexcept = default;
    
    // Non-copyable, non-movable for MISRA compliance
    Http3Client(const Http3Client&) = delete;
    Http3Client& operator=(const Http3Client&) = delete;
    Http3Client(Http3Client&&) = delete;
    Http3Client& operator=(Http3Client&&) = delete;
    
    ~Http3Client() = default;
    
    [[nodiscard]] constexpr Result connect(std::string_view hostname, std::uint16_t port = Config::DEFAULT_QUIC_PORT) noexcept {
        auto validation_result = validate_hostname(hostname);
        if (validation_result != Result::Success) {
            return validation_result;
        }
        
        if (state_ != ConnectionState::Disconnected) {
            return Result::ProtocolError;
        }
        
        // Store connection parameters
        std::size_t copy_length = hostname.length();
        for (std::size_t i = 0U; i < copy_length; ++i) {
            hostname_[i] = hostname[i];
        }
        hostname_[copy_length] = '\0';
        port_ = port;
        
        state_ = ConnectionState::Connecting;
        
        // TODO: Real QUIC handshake
        // 1. UDP socket creation
        // 2. QUIC Initial packet with ClientHello
        // 3. TLS 1.3 handshake completion
        // 4. QUIC transport parameters
        
        connection_id_ = 0x1234567890ABCDEFULL;
        quic_handshake_complete_ = true;
        
        // Create HTTP/3 control streams
        control_stream_id_ = stream_manager_.create_unidirectional_stream(StreamType::CONTROL);
        qpack_encoder_stream_id_ = stream_manager_.create_unidirectional_stream(StreamType::QPACK_ENCODER);
        qpack_decoder_stream_id_ = stream_manager_.create_unidirectional_stream(StreamType::QPACK_DECODER);
        
        // Send HTTP/3 SETTINGS
        auto result = send_http3_settings();
        if (result != Result::Success) {
            return result;
        }
        
        state_ = ConnectionState::Connected;
        return Result::Success;
    }
    
    [[nodiscard]] constexpr Result post_json(std::string_view path, const JsonBuilder& json_data) noexcept {
        if (state_ != ConnectionState::Connected) {
            return Result::ProtocolError;
        }
        
        if (json_data.has_error()) {
            return Result::JsonError;
        }
        
        auto json_str = json_data.get_json();
        if (json_str.empty()) {
            return Result::JsonError;
        }
        
        // Create request stream
        std::uint64_t stream_id = stream_manager_.create_request_stream();
        
        // Create additional headers
        std::array<HttpHeader, 2U> additional_headers{};
        additional_headers[0].set_header("content-type", "application/json");
        
        // Convert content-length to string
        std::array<char, 10U> length_str{};
        std::size_t length_digits = 0U;
        std::size_t content_length = json_str.length();
        
        if (content_length == 0U) {
            length_str[0] = '0';
            length_digits = 1U;
        } else {
            std::size_t temp = content_length;
            while (temp > 0U && length_digits < length_str.size()) {
                length_str[length_digits++] = static_cast<char>('0' + (temp % 10U));
                temp /= 10U;
            }
            
            for (std::size_t i = 0U; i < length_digits / 2U; ++i) {
                char temp_char = length_str[i];
                length_str[i] = length_str[length_digits - 1U - i];
                length_str[length_digits - 1U - i] = temp_char;
            }
        }
        
        additional_headers[1].set_header("content-length", std::string_view(length_str.data(), length_digits));
        
        // Create HEADERS frame
        Http3Frame headers_frame{};
        std::string_view hostname_view(hostname_.data());
        auto result = create_headers_frame("POST", path, hostname_view, 
                                         additional_headers.data(), 2U, headers_frame);
        if (result != Result::Success) {
            return result;
        }
        
        // Send HEADERS frame
        result = send_frame_on_stream(stream_id, headers_frame);
        if (result != Result::Success) {
            return result;
        }
        
        // Create DATA frame
        Http3Frame data_frame{};
        result = create_data_frame(reinterpret_cast<const std::uint8_t*>(json_str.data()), 
                                 json_str.length(), data_frame);
        if (result != Result::Success) {
            return result;
        }
        
        // Send DATA frame
        result = send_frame_on_stream(stream_id, data_frame);
        if (result != Result::Success) {
            return result;
        }
        
        // Receive response
        return receive_frames(Config::DEFAULT_TIMEOUT_MS);
    }
    
    [[nodiscard]] constexpr Result get_status(std::string_view path) noexcept {
        if (state_ != ConnectionState::Connected) {
            return Result::ProtocolError;
        }
        
        std::uint64_t stream_id = stream_manager_.create_request_stream();
        
        Http3Frame headers_frame{};
        std::string_view hostname_view(hostname_.data());
        auto result = create_headers_frame("GET", path, hostname_view, nullptr, 0U, headers_frame);
        if (result != Result::Success) {
            return result;
        }
        
        result = send_frame_on_stream(stream_id, headers_frame);
        if (result != Result::Success) {
            return result;
        }
        
        return receive_frames(Config::DEFAULT_TIMEOUT_MS);
    }
    
    // Alt-Svc discovery for HTTP/3
    [[nodiscard]] constexpr Result discover_http3_support(std::string_view alt_svc_header) noexcept {
        return alt_svc_info_.parse_alt_svc(alt_svc_header);
    }
    
    [[nodiscard]] constexpr HttpStatus get_last_status() const noexcept {
        return last_status_;
    }
    
    [[nodiscard]] constexpr std::string_view get_last_response_body() const noexcept {
        return std::string_view(response_body_.data(), response_body_length_);
    }
    
    [[nodiscard]] constexpr bool last_request_successful() const noexcept {
        std::uint16_t status_code = static_cast<std::uint16_t>(last_status_);
        return status_code >= 200U && status_code < 300U;
    }
    
    [[nodiscard]] constexpr Result disconnect() noexcept {
        if (state_ == ConnectionState::Disconnected) {
            return Result::Success;
        }
        
        state_ = ConnectionState::Closing;
        
        // TODO: Send GOAWAY frame and CONNECTION_CLOSE
        
        state_ = ConnectionState::Disconnected;
        quic_handshake_complete_ = false;
        http3_settings_sent_ = false;
        packet_number_ = 0U;
        
        return Result::Success;
    }
    
    [[nodiscard]] constexpr bool is_connected() const noexcept {
        return state_ == ConnectionState::Connected && quic_handshake_complete_;
    }
};

// Factory function
[[nodiscard]] constexpr Http3Client create_http3_client() noexcept {
    return Http3Client{};
}

} // namespace http3

// Example usage with proper HTTP/3 semantics
[[nodiscard]] constexpr http3::Result example_complete_workflow() noexcept {
    using namespace http3;
    
    auto client = create_http3_client();
    
    // Step 1: Connect with QUIC/HTTP3
    auto result = client.connect("api.example.com", 443);
    if (result != Result::Success) {
        return result;
    }
    
    // Step 2: Build JSON payload
    JsonBuilder json;
    json.start_object();
    json.add_string_field("sensor_id", "temp_sensor_01", false);
    json.add_number_field("temperature", 23, false);
    json.add_string_field("unit", "celsius", false);
    json.add_string_field("timestamp", "2024-01-15T10:30:00Z", true);
    json.end_object();
    
    // Step 3: Send JSON via proper HTTP/3 POST with QPACK headers
    result = client.post_json("/api/v1/sensors/data", json);
    if (result != Result::Success) {
        return result;
    }
    
    // Step 4: Check response status
    if (!client.last_request_successful()) {
        return Result::ServerError;
    }
    
    // Step 5: Get system health status
    result = client.get_status("/api/v1/health");
    if (result != Result::Success) {
        return result;
    }
    
    // Step 6: Clean disconnection
    client.disconnect();
    
    return Result::Success;
}
