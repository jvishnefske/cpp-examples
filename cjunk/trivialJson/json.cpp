#include <json.hpp>
#include <algorithm>
#include <ostream>
#include <sstream>
#include <vector>
#include <string>
#include <fmt/core.h>
#include <iostream>

// todo: add operator<< for each serialization type bson, json, etc.
// todo convert generators to trivial constructors
// todo move more includes into implementation: consider using a unique_ptr to variant, and fwd declaring variant.
// todo add json objects of type std::map<string, JsonNode>
// todo consider adding bson types where they make sense for other serialization methods decimal128 datetime in milliseconds
// todo make constexpr operator""json
// todo is it possble to use compile time schemas and create node.member which links to {"member": value}
struct JsonVisitor : Node {
    std::string operator()(const ListPtr &o) {
        if (!o) {
            // for debugging
            return "nullptr!!!";
        }
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for (auto &_o: *o) {
            if (first) {
                first = false;
            } else {
                oss << ",";
            }
            //oss << "todo 1";//_o.visit(*this);
            // use friend declaration to vist the private storage in Node::visit
            oss << _o.visit(*this);
        }
        oss << "]";
        return oss.str();
    }

    std::string operator()(const std::string &o) const {
        return std::string("\"") + o + std::string("\"");
    }
    
    std::string operator()(const Node::ObjectPtr &o) {
        if (!o) {
            return "null";
        }
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (const auto& [key, value] : *o) {
            if (first) {
                first = false;
            } else {
                oss << ",";
            }
            oss << "\"" << key << "\":" << value.serialize();
        }
        oss << "}";
        return oss.str();
    }

    std::string operator()(const Node::SmallString str) {
        std::cout << "CharArray" << typeid(str).name() << std::endl;

        // explicitly specify the string length since str is not always null terminated.
        // note that this will always make std::string::size always return 8 including any optional terminating characters.
        // c_str is used to truncate based on a null terminator which std::string provides.
        return std::string("\"") + std::string(str.data(), str.size()).c_str() + std::string("\"");
    }


    std::string operator()(const std::string& obj) {
        std::cout << "constructable" << typeid(obj).name() << std::endl;
        return "\"" + obj + "\"";
    }

    template<typename T>
    std::string operator()(T o) const {
        std::cout << "to_string" << std::endl;
        return std::to_string(o);
    }
};

std::ostream& operator<<(std::ostream &os, const Node &object) {
    os << object.serialize();
    return os;
}

std::string Node::serialize() const {
    std::ostringstream oss;
    JsonVisitor v;
    //return visit(v);
    return std::visit(v, this->_storage);
}

// Simple JSON parser implementation
namespace {
    class JsonParser {
    private:
        std::string json_;
        size_t pos_;

        void skip_whitespace() {
            while (pos_ < json_.size() && std::isspace(json_[pos_])) {
                ++pos_;
            }
        }

        std::string parse_string() {
            if (json_[pos_] != '"') {
                throw std::runtime_error("Expected '\"' at start of string");
            }
            ++pos_; // Skip opening quote
            
            std::string result;
            while (pos_ < json_.size() && json_[pos_] != '"') {
                if (json_[pos_] == '\\') {
                    ++pos_; // Skip escape character
                    if (pos_ >= json_.size()) {
                        throw std::runtime_error("Unexpected end of string");
                    }
                    switch (json_[pos_]) {
                        case '"': result += '"'; break;
                        case '\\': result += '\\'; break;
                        case '/': result += '/'; break;
                        case 'b': result += '\b'; break;
                        case 'f': result += '\f'; break;
                        case 'n': result += '\n'; break;
                        case 'r': result += '\r'; break;
                        case 't': result += '\t'; break;
                        default:
                            result += json_[pos_]; // Just add the character
                    }
                } else {
                    result += json_[pos_];
                }
                ++pos_;
            }
            
            if (pos_ >= json_.size()) {
                throw std::runtime_error("Unterminated string");
            }
            ++pos_; // Skip closing quote
            return result;
        }

        Node parse_number() {
            std::string num_str;
            bool is_double = false;
            
            while (pos_ < json_.size() && 
                   (std::isdigit(json_[pos_]) || json_[pos_] == '.' || 
                    json_[pos_] == '-' || json_[pos_] == '+' || 
                    json_[pos_] == 'e' || json_[pos_] == 'E')) {
                if (json_[pos_] == '.') {
                    is_double = true;
                }
                num_str += json_[pos_];
                ++pos_;
            }
            
            if (is_double) {
                return Node(std::stod(num_str));
            } else {
                return Node(static_cast<int64_t>(std::stoll(num_str)));
            }
        }

        Node parse_object() {
            if (json_[pos_] != '{') {
                throw std::runtime_error("Expected '{'");
            }
            ++pos_;
            
            auto obj = std::make_shared<Node::Object>();
            skip_whitespace();
            
            if (pos_ < json_.size() && json_[pos_] == '}') {
                ++pos_;
                return Node(obj);
            }
            
            while (true) {
                skip_whitespace();
                if (pos_ >= json_.size()) {
                    throw std::runtime_error("Unexpected end of object");
                }
                
                std::string key = parse_string();
                skip_whitespace();
                
                if (pos_ >= json_.size() || json_[pos_] != ':') {
                    throw std::runtime_error("Expected ':' after key");
                }
                ++pos_;
                
                skip_whitespace();
                Node value = parse_value();
                (*obj)[key] = value;
                
                skip_whitespace();
                if (pos_ >= json_.size()) {
                    throw std::runtime_error("Unexpected end of object");
                }
                
                if (json_[pos_] == '}') {
                    ++pos_;
                    break;
                } else if (json_[pos_] == ',') {
                    ++pos_;
                } else {
                    throw std::runtime_error("Expected ',' or '}' in object");
                }
            }
            
            return Node(obj);
        }

        Node parse_array() {
            if (json_[pos_] != '[') {
                throw std::runtime_error("Expected '['");
            }
            ++pos_;
            
            auto arr = std::make_shared<Node::List>();
            skip_whitespace();
            
            if (pos_ < json_.size() && json_[pos_] == ']') {
                ++pos_;
                return Node(arr);
            }
            
            while (true) {
                skip_whitespace();
                arr->push_back(parse_value());
                skip_whitespace();
                
                if (pos_ >= json_.size()) {
                    throw std::runtime_error("Unexpected end of array");
                }
                
                if (json_[pos_] == ']') {
                    ++pos_;
                    break;
                } else if (json_[pos_] == ',') {
                    ++pos_;
                } else {
                    throw std::runtime_error("Expected ',' or ']' in array");
                }
            }
            
            return Node(arr);
        }

        Node parse_value() {
            skip_whitespace();
            
            if (pos_ >= json_.size()) {
                throw std::runtime_error("Unexpected end of input");
            }
            
            char c = json_[pos_];
            
            if (c == '"') {
                return Node(parse_string());
            } else if (c == '{') {
                return parse_object();
            } else if (c == '[') {
                return parse_array();
            } else if (std::isdigit(c) || c == '-') {
                return parse_number();
            } else if (json_.substr(pos_, 4) == "true") {
                pos_ += 4;
                return Node(static_cast<int64_t>(1));
            } else if (json_.substr(pos_, 5) == "false") {
                pos_ += 5;
                return Node(static_cast<int64_t>(0));
            } else if (json_.substr(pos_, 4) == "null") {
                pos_ += 4;
                return Node();
            } else {
                throw std::runtime_error("Unexpected character: " + std::string(1, c));
            }
        }

    public:
        JsonParser(const std::string& json) : json_(json), pos_(0) {}
        
        Node parse() {
            return parse_value();
        }
    };
}

Node parse_json(const std::string& json_string) {
    JsonParser parser(json_string);
    return parser.parse();
}
