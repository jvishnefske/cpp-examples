#include <variant>
#include <regex>
#include <vector> // Added missing include for std::vector
#include <string> // Added missing include for std::string
#include <map> // Added missing include for std::map
#include <iostream> // Added missing include for std::cout
#include <memory> // For std::unique_ptr
#include <stdexcept> // For std::runtime_error
#include <cctype> // For std::isspace, std::isdigit

// Forward declaration for recursive types
struct JsonValue;

// Define types for array and object to break recursive dependency in std::variant
using JsonArray = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;

struct JsonValue {
    // The variant holds the actual data.
    // Using unique_ptr for recursive types (array, object) to avoid infinite size.
    std::variant<
        std::monostate, // For null
        bool,
        long long,      // For integers
        double,         // For floating-point numbers
        std::string,
        std::unique_ptr<JsonArray>,
        std::unique_ptr<JsonObject>
    > data;

    // Add a default destructor as per cppcoreguidelines-special-member-functions
    ~JsonValue() = default;

    // Constructors for each type
    JsonValue() : data(std::monostate{}) {} // Default constructor for null
    JsonValue(bool b) : data(b) {}
    JsonValue(long long i) : data(i) {}
    JsonValue(double d) : data(d) {}
    JsonValue(std::string s) : data(std::move(s)) {}
    JsonValue(const char* s) : data(std::string(s)) {}
    JsonValue(JsonArray arr) : data(std::make_unique<JsonArray>(std::move(arr))) {}
    JsonValue(JsonObject obj) : data(std::make_unique<JsonObject>(std::move(obj))) {}

    // Copy constructor and assignment operator for deep copy of unique_ptrs
    JsonValue(const JsonValue& other) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<JsonArray>>) {
                data = std::make_unique<JsonArray>(*arg);
            } else if constexpr (std::is_same_v<T, std::unique_ptr<JsonObject>>) {
                data = std::make_unique<JsonObject>(*arg);
            } else {
                data = arg;
            }
        }, other.data);
    }

    JsonValue& operator=(const JsonValue& other) {
        if (this != &other) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::unique_ptr<JsonArray>>) {
                    data = std::make_unique<JsonArray>(*arg);
                } else if constexpr (std::is_same_v<T, std::unique_ptr<JsonObject>>) {
                    data = std::make_unique<JsonObject>(*arg);
                } else {
                    data = arg;
                }
            }, other.data);
        }
        return *this;
    }

    // Move constructor and assignment operator (default is fine for unique_ptr)
    JsonValue(JsonValue&&) noexcept = default;
    JsonValue& operator=(JsonValue&&) noexcept = default;

    // Helper to get type name (for debugging)
    std::string type_name() const {
        return std::visit([](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) return "null";
            else if constexpr (std::is_same_v<T, bool>) return "bool";
            else if constexpr (std::is_same_v<T, long long>) return "long long";
            else if constexpr (std::is_same_v<T, double>) return "double";
            else if constexpr (std::is_same_v<T, std::string>) return "string";
            else if constexpr (std::is_same_v<T, std::unique_ptr<JsonArray>>) return "array";
            else if constexpr (std::is_same_v<T, std::unique_ptr<JsonObject>>) return "object";
            return "unknown";
        }, data);
    }

    // Accessors (simplified for this rewrite, can be expanded with operator[] etc.)
    bool is_null() const { return std::holds_alternative<std::monostate>(data); }
    bool is_bool() const { return std::holds_alternative<bool>(data); }
    bool is_long() const { return std::holds_alternative<long long>(data); }
    bool is_double() const { return std::holds_alternative<double>(data); }
    bool is_string() const { return std::holds_alternative<std::string>(data); }
    bool is_array() const { return std::holds_alternative<std::unique_ptr<JsonArray>>(data); }
    bool is_object() const { return std::holds_alternative<std::unique_ptr<JsonObject>>(data); }

    bool get_bool() const { return std::get<bool>(data); }
    long long get_long() const { return std::get<long long>(data); }
    double get_double() const { return std::get<double>(data); }
    const std::string& get_string() const { return std::get<std::string>(data); }
    const JsonArray& get_array() const { return *std::get<std::unique_ptr<JsonArray>>(data); }
    const JsonObject& get_object() const { return *std::get<std::unique_ptr<JsonObject>>(data); }

    // For non-const access (if needed)
    JsonArray& get_array() { return *std::get<std::unique_ptr<JsonArray>>(data); }
    JsonObject& get_object() { return *std::get<std::unique_ptr<JsonObject>>(data); }
};

// --- JSON Parser Implementation ---
class JsonParser {
private:
    std::string_view json_str;
    size_t pos = 0; // Initialize pos with default member initializer

    void skip_whitespace() {
        while (pos < json_str.length() && std::isspace(json_str[pos])) {
            pos++;
        }
    }

    char peek() const {
        if (pos >= json_str.length()) {
            throw std::runtime_error("Unexpected end of input");
        }
        return json_str[pos];
    }

    char consume() {
        if (pos >= json_str.length()) {
            throw std::runtime_error("Unexpected end of input");
        }
        return json_str[pos++];
    }

    void expect(char c) {
        skip_whitespace();
        if (peek() == c) {
            consume();
        } else {
            throw std::runtime_error("Expected '" + std::string(1, c) + "', got '" + std::string(1, peek()) + "' at position " + std::to_string(pos));
        }
    }

    std::string parse_string() {
        expect('"');
        std::string s;
        while (peek() != '"') {
            char c = consume();
            if (c == '\\') {
                char escaped_char = consume();
                switch (escaped_char) {
                    case '"': s += '"'; break;
                    case '\\': s += '\\'; break;
                    case '/': s += '/'; break;
                    case 'b': s += '\b'; break;
                    case 'f': s += '\f'; break;
                    case 'n': s += '\n'; break;
                    case 'r': s += '\r'; break;
                    case 't': s += '\t'; break;
                    // TODO: Handle \uXXXX unicode escapes if full JSON compliance is needed
                    default: throw std::runtime_error("Invalid escape sequence");
                }
            } else {
                s += c;
            }
        }
        expect('"');
        return s;
    }

    JsonValue parse_number() {
        size_t start_pos = pos;
        // Handle optional sign
        if (peek() == '-') {
            consume();
        }
        // Parse integer part
        while (pos < json_str.length() && std::isdigit(json_str[pos])) {
            consume();
        }
        bool is_double = false;
        // Parse fractional part
        if (peek() == '.') {
            is_double = true;
            consume();
            while (pos < json_str.length() && std::isdigit(json_str[pos])) {
                consume();
            }
        }
        // Parse exponent part
        if (peek() == 'e' || peek() == 'E') {
            is_double = true;
            consume();
            if (peek() == '+' || peek() == '-') {
                consume();
            }
            while (pos < json_str.length() && std::isdigit(json_str[pos])) {
                consume();
            }
        }

        std::string num_str = std::string(json_str.substr(start_pos, pos - start_pos));
        if (is_double) {
            return JsonValue(std::stod(num_str));
        } else {
            return JsonValue(std::stoll(num_str));
        }
    }

    JsonValue parse_literal(const std::string& literal, JsonValue value) {
        for (char expected_char : literal) {
            if (pos >= json_str.length() || json_str[pos] != expected_char) {
                throw std::runtime_error("Expected literal '" + literal + "'");
            }
            pos++;
        }
        return value;
    }

    JsonValue parse_array() {
        expect('[');
        JsonArray arr;
        skip_whitespace();
        if (peek() != ']') {
            while (true) {
                arr.push_back(parse_value());
                skip_whitespace();
                if (peek() == ',') {
                    consume();
                } else if (peek() == ']') {
                    break;
                } else {
                    throw std::runtime_error("Expected ',' or ']' in array");
                }
            }
        }
        expect(']');
        return JsonValue(std::move(arr));
    }

    JsonValue parse_object() {
        expect('{');
        JsonObject obj;
        skip_whitespace();
        if (peek() != '}') {
            while (true) {
                std::string key = parse_string();
                expect(':');
                JsonValue value = parse_value();
                obj[key] = std::move(value);
                skip_whitespace();
                if (peek() == ',') {
                    consume();
                } else if (peek() == '}') {
                    break;
                } else {
                    throw std::runtime_error("Expected ',' or '}' in object");
                }
            }
        }
        expect('}');
        return JsonValue(std::move(obj));
    }

    JsonValue parse_value() {
        skip_whitespace();
        char current_char = peek();
        if (current_char == '"') {
            return parse_string();
        } else if (current_char == '-' || std::isdigit(current_char)) {
            return parse_number();
        } else if (current_char == '[') {
            return parse_array();
        } else if (current_char == '{') {
            return parse_object();
        } else if (current_char == 't') {
            return parse_literal("true", JsonValue(true));
        } else if (current_char == 'f') {
            return parse_literal("false", JsonValue(false));
        } else if (current_char == 'n') {
            return parse_literal("null", JsonValue()); // Default constructor is null
        } else {
            throw std::runtime_error("Unexpected character: '" + std::string(1, current_char) + "' at position " + std::to_string(pos));
        }
    }

public:
    JsonParser(std::string_view json) : json_str(json), pos(0) {}

    JsonValue parse() {
        JsonValue result = parse_value();
        skip_whitespace();
        if (pos != json_str.length()) {
            throw std::runtime_error("Extra characters after JSON value at position " + std::to_string(pos));
        }
        return result;
    }
};

// --- JSON Serialization (for testing/output) ---
std::string serialize_json(const JsonValue& value, int indent_level = 0) {
    std::string result;
    std::string indent_str(indent_level * 2, ' '); // 2 spaces per indent level

    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            result += "null";
        } else if constexpr (std::is_same_v<T, bool>) {
            result += (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, long long>) {
            result += std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            // Use std::to_string for doubles, might need more precision control for real JSON
            result += std::to_string(arg);
            // Remove trailing zeros and decimal point if it's an integer value
            if (result.find('.') != std::string::npos) {
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                if (result.back() == '.') {
                    result.pop_back();
                }
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Basic string escaping (only for double quotes and backslashes)
            result += "\"";
            for (char c : arg) {
                if (c == '"') result += "\\\"";
                else if (c == '\\') result += "\\\\";
                else result += c;
            }
            result += "\"";
        } else if constexpr (std::is_same_v<T, std::unique_ptr<JsonArray>>) {
            result += "[\n";
            bool first = true;
            for (const auto& item : *arg) {
                if (!first) result += ",\n";
                result += indent_str + "  " + serialize_json(item, indent_level + 1);
                first = false;
            }
            result += "\n" + indent_str + "]";
        } else if constexpr (std::is_same_v<T, std::unique_ptr<JsonObject>>) {
            result += "{\n";
            bool first = true;
            for (const auto& pair : *arg) {
                if (!first) result += ",\n";
                result += indent_str + "  \"" + pair.first + "\": " + serialize_json(pair.second, indent_level + 1);
                first = false;
            }
            result += "\n" + indent_str + "}";
        }
    }, value.data);
    return result;
}

// --- Main function for testing ---
int main(){
    std::cout << "--- Testing JSON Parser ---" << std::endl;

    // Test cases
    std::string json1 = R"([1, 2, 3])";
    std::string json2 = R"({"name": "Alice", "age": 30, "isStudent": false, "grades": [90, 85.5, 92], "address": null})";
    std::string json3 = R"({"key_with_escapes": "value with \"quotes\" and \\backslashes\\ and \nnewlines"})";
    std::string json4 = R"([{"a":1}, {"b":2}])";
    std::string json5 = R"(12345)";
    std::string json6 = R"(3.14159)";
    std::string json7 = R"(-1.2e-5)";
    std::string json8 = R"(true)";
    std::string json9 = R"(null)";
    std::string json10 = R"({})";
    std::string json11 = R"([])";
    std::string json12 = R"({"nested": {"object": [1, {"array_in_object": true}]}})";

    std::vector<std::string> test_jsons = {
        json1, json2, json3, json4, json5, json6, json7, json8, json9, json10, json11, json12
    };

    for (const auto& json_str : test_jsons) {
        try {
            std::cout << "\nParsing: " << json_str << std::endl;
            JsonParser parser(json_str);
            JsonValue parsed_value = parser.parse();
            std::cout << "Parsed successfully. Type: " << parsed_value.type_name() << std::endl;
            std::cout << "Serialized back:\n" << serialize_json(parsed_value) << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }
    }

    // Test invalid JSON
    std::cout << "\n--- Testing Invalid JSON ---" << std::endl;
    std::string invalid_json1 = R"([1, 2,])"; // Trailing comma
    std::string invalid_json2 = R"({"a":1 "b":2})"; // Missing comma
    std::string invalid_json3 = R"({"a":})"; // Missing value
    std::string invalid_json4 = R"(invalid)"; // Not a valid JSON value
    std::string invalid_json5 = R"([1,2,3 extra])"; // Extra characters

    std::vector<std::string> invalid_jsons = {
        invalid_json1, invalid_json2, invalid_json3, invalid_json4, invalid_json5
    };

    for (const auto& json_str : invalid_jsons) {
        try {
            std::cout << "\nParsing invalid: " << json_str << std::endl;
            JsonParser parser(json_str);
            JsonValue parsed_value = parser.parse();
            std::cout << "Parsed successfully (unexpected for invalid JSON)." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Caught expected error: " << e.what() << std::endl;
        }
    }

    return 0;
}
