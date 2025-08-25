#pragma once
#include <variant>
#include <iosfwd>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <memory>
#include <array>
#include <type_traits>
#include <stdexcept>
#include <sstream>
#include <map>
#include <cstring>

struct Node {
    using SmallString = std::array<char, 8>;
    using List = std::vector<Node>;
    using ListPtr = std::shared_ptr<List>;
    using Object = std::map<std::string, Node>;
    using ObjectPtr = std::shared_ptr<Object>;
    // reorder to put something sane for trivial construction.
    using Storage = std::variant<SmallString, int64_t, double, ListPtr, ObjectPtr, std::string>;

    template<typename Integer, std::enable_if_t<std::is_integral_v<Integer> && !std::is_same_v<Integer, bool>, bool> = true>
    constexpr explicit Node(const Integer i): _storage{static_cast<int64_t>(i)} {}

    // Explicit bool constructor
    constexpr explicit Node(bool b): _storage{static_cast<int64_t>(b ? 1 : 0)} {}

    template<typename T, std::enable_if_t<std::is_convertible_v<T, Storage> && !std::is_integral_v<T>, bool> = true>
    constexpr explicit Node(const T obj): _storage(obj) {}
    // only needed for small strings since char* is not convertable to SmallString
//    template<typename T, std::enable_if_t<std::is_convertible_v<T, std::string>, bool> = true>
//    explicit Node(T obj)
//        //: _storage(generator(obj)._storage)
//        {
//            _storage.template emplace<std::array>({})
//        }


// todo try to make this work
//    constexpr Node(const Node&) = default;
//    constexpr Node(Node&&) = default;
//    constexpr Node& operator=(const Node&) = default;
//    constexpr Node& operator=(Node&&) = default;
//    constexpr bool operator==(const Node& rhs) const {
//        return _storage == rhs._storage;
//    }
    constexpr bool operator!=(const Node &rhs) const {
        return _storage != rhs._storage;
    }

    // declare  JsonVisitor as friend so it can access private members
    template<typename Visitor>
    friend auto visit(Visitor &visitor, const Node &node) -> decltype(visitor(node)) {
        return visitor(node);
    }

    constexpr size_t length(const char *s) {
        return *s ? length(s + 1) + 1 : 1;
    }

    explicit Node(const char *s) {
        size_t len = std::strlen(s);
        if (len < sizeof(SmallString)) {
            SmallString d{};
            std::copy_n(s, len + 1, d.data()); // Include null terminator
            _storage = d;
        } else {
            _storage = std::string(s);
        }
    }

    explicit Node(const std::string& s) {
        if (s.size() < sizeof(SmallString)) {
            SmallString d{};
            std::copy_n(s.c_str(), s.size() + 1, d.data());
            _storage = d;
        } else {
            _storage = s;
        }
    }

    // Constructor for objects
    explicit Node(const Object& obj) : _storage(std::make_shared<Object>(obj)) {}
    explicit Node(ObjectPtr obj) : _storage(obj) {}
    
    // Constructor for arrays
    explicit Node(const List& arr) : _storage(std::make_shared<List>(arr)) {}
    explicit Node(ListPtr arr) : _storage(arr) {}

    // Commented out broken constructor that used removed generator function
    /*
    template<typename ...Args>
    explicit Node(Args const &... args){
        static_assert(std::is_trivially_constructible<std::vector<Node>, Args...>::value , "possible to contruct from container");
        //auto generator = [](auto arg){return Node(arg);};
        std::vector<Node> container{ generator(args) ...  };
        //(  container.push_back(args)  ... );
        _storage.emplace(std::forward<Args...>(args) ...);
    }
    */

    Node() = default;

    /**
     *
     *
     * @tparam Visitor
     * @param v
     * @return
     */
    template<typename Visitor>
    auto visit(Visitor v){
        return std::visit(v, _storage);
    }
    
    // Access methods for different types
    bool is_string() const { return std::holds_alternative<SmallString>(_storage) || std::holds_alternative<std::string>(_storage); }
    bool is_number() const { return std::holds_alternative<int64_t>(_storage) || std::holds_alternative<double>(_storage); }
    bool is_object() const { return std::holds_alternative<ObjectPtr>(_storage); }
    bool is_array() const { return std::holds_alternative<ListPtr>(_storage); }
    bool is_bool() const { 
        if (auto* i = std::get_if<int64_t>(&_storage)) {
            return *i == 0 || *i == 1;  // Treat 0/1 as boolean
        }
        return false;
    }
    
    std::string as_string() const {
        if (auto* small = std::get_if<SmallString>(&_storage)) {
            return std::string(small->data());
        }
        if (auto* str = std::get_if<std::string>(&_storage)) {
            return *str;
        }
        throw std::runtime_error("Node is not a string");
    }
    
    int64_t as_int() const {
        if (auto* i = std::get_if<int64_t>(&_storage)) {
            return *i;
        }
        throw std::runtime_error("Node is not an integer");
    }
    
    bool as_bool() const {
        if (auto* i = std::get_if<int64_t>(&_storage)) {
            return *i != 0;  // Convert 0 to false, non-zero to true
        }
        throw std::runtime_error("Node is not a boolean");
    }
    
    double as_double() const {
        if (auto* d = std::get_if<double>(&_storage)) {
            return *d;
        }
        throw std::runtime_error("Node is not a double");
    }
    
    Object& as_object() {
        if (auto* obj = std::get_if<ObjectPtr>(&_storage)) {
            return **obj;
        }
        throw std::runtime_error("Node is not an object");
    }
    
    const Object& as_object() const {
        if (auto* obj = std::get_if<ObjectPtr>(&_storage)) {
            return **obj;
        }
        throw std::runtime_error("Node is not an object");
    }
    
    List& as_array() {
        if (auto* arr = std::get_if<ListPtr>(&_storage)) {
            return **arr;
        }
        throw std::runtime_error("Node is not an array");
    }
    
    const List& as_array() const {
        if (auto* arr = std::get_if<ListPtr>(&_storage)) {
            return **arr;
        }
        throw std::runtime_error("Node is not an array");
    }
    
    // Array access by index
    Node& operator[](size_t index) {
        if (auto* arr = std::get_if<ListPtr>(&_storage)) {
            if (index < (*arr)->size()) {
                return (**arr)[index];
            }
        }
        throw std::runtime_error("Node is not an array or index out of bounds");
    }
    
    const Node& operator[](size_t index) const {
        if (auto* arr = std::get_if<ListPtr>(&_storage)) {
            if (index < (*arr)->size()) {
                return (**arr)[index];
            }
        }
        throw std::runtime_error("Node is not an array or index out of bounds");
    }
    
    // Object access by key
    Node& operator[](const std::string& key) {
        if (auto* obj = std::get_if<ObjectPtr>(&_storage)) {
            return (**obj)[key];  // Creates key if it doesn't exist
        }
        throw std::runtime_error("Node is not an object");
    }
    
    const Node& operator[](const std::string& key) const {
        if (auto* obj = std::get_if<ObjectPtr>(&_storage)) {
            auto it = (*obj)->find(key);
            if (it != (*obj)->end()) {
                return it->second;
            }
        }
        throw std::runtime_error("Node is not an object or key not found");
    }
    
    // Convenience overload for string literals
    Node& operator[](const char* key) {
        return (*this)[std::string(key)];
    }
    
    const Node& operator[](const char* key) const {
        return (*this)[std::string(key)];
    }
    
    // Add size() method for arrays and objects
    size_t size() const {
        if (auto* arr = std::get_if<ListPtr>(&_storage)) {
            return (*arr)->size();
        }
        if (auto* obj = std::get_if<ObjectPtr>(&_storage)) {
            return (*obj)->size();
        }
        return 0;  // Scalar values have size 0
    }
    
    std::string serialize() const;
protected:
    Storage _storage;

    /**
     * These generate functions were included simply because the MSVC++ gave me compile errors
     * when attempting to add parameter pack expansion to a Node constructor.
     * Removing unused generator functions to improve coverage.
     */
};

// how to differentiate serialization methods
std::ostream& operator<<(std::ostream &os, const Node &obj);

// JSON parsing function - replaces Json::Reader functionality
Node parse_json(const std::string& json_string);

// JsonBuilder class for constructing JSON objects
class JsonBuilder {
private:
    Node::ObjectPtr current_object_;
    Node::ListPtr current_array_;
    std::vector<Node::ObjectPtr> object_stack_;
    std::vector<Node::ListPtr> array_stack_;
    bool in_object_;

public:
    JsonBuilder() : in_object_(false) {}
    
    JsonBuilder& start_object() {
        current_object_ = std::make_shared<Node::Object>();
        object_stack_.push_back(current_object_);
        in_object_ = true;
        return *this;
    }
    
    JsonBuilder& end_object() {
        if (!in_object_ || object_stack_.empty()) {
            throw std::runtime_error("end_object() called without start_object()");
        }
        object_stack_.pop_back();
        in_object_ = !object_stack_.empty();
        return *this;
    }
    
    JsonBuilder& add_string_field(const std::string& key, const std::string& value, bool /*last*/ = false) {
        if (!in_object_ || !current_object_) {
            throw std::runtime_error("add_string_field() called outside of object");
        }
        (*current_object_)[key] = Node(value);
        return *this;
    }
    
    JsonBuilder& add_int_field(const std::string& key, int64_t value, bool /*last*/ = false) {
        if (!in_object_ || !current_object_) {
            throw std::runtime_error("add_int_field() called outside of object");
        }
        (*current_object_)[key] = Node(value);
        return *this;
    }
    
    JsonBuilder& add_double_field(const std::string& key, double value, bool /*last*/ = false) {
        if (!in_object_ || !current_object_) {
            throw std::runtime_error("add_double_field() called outside of object");
        }
        (*current_object_)[key] = Node(value);
        return *this;
    }
    
    JsonBuilder& add_bool_field(const std::string& key, bool value, bool /*last*/ = false) {
        if (!in_object_ || !current_object_) {
            throw std::runtime_error("add_bool_field() called outside of object");
        }
        (*current_object_)[key] = Node(value);
        return *this;
    }
    
    JsonBuilder& start_array() {
        current_array_ = std::make_shared<Node::List>();
        array_stack_.push_back(current_array_);
        in_object_ = false;
        return *this;
    }
    
    JsonBuilder& end_array() {
        if (in_object_ || array_stack_.empty()) {
            throw std::runtime_error("end_array() called without start_array()");
        }
        // Keep current_array_ pointing to the built array, just pop from stack
        array_stack_.pop_back();
        in_object_ = !object_stack_.empty();
        return *this;
    }
    
    JsonBuilder& add_string_value(const std::string& value) {
        if (in_object_ || !current_array_) {
            throw std::runtime_error("add_string_value() called outside of array");
        }
        current_array_->push_back(Node(value));
        return *this;
    }
    
    JsonBuilder& add_int_value(int64_t value) {
        if (in_object_ || !current_array_) {
            throw std::runtime_error("add_int_value() called outside of array");
        }
        current_array_->push_back(Node(value));
        return *this;
    }
    
    JsonBuilder& add_bool_value(bool value) {
        if (in_object_ || !current_array_) {
            throw std::runtime_error("add_bool_value() called outside of array");
        }
        current_array_->push_back(Node(value));
        return *this;
    }
    
    Node build() {
        if (current_object_) {
            return Node(current_object_);
        } else if (current_array_) {
            return Node(current_array_);
        }
        return Node(); // Empty node
    }
};

// Compatibility layer for existing JsonCpp usage
namespace Json {
    using Value = Node;
    
    class Reader {
    public:
        bool parse(const std::string& json, Node& root) {
            try {
                root = parse_json(json);
                return true;
            } catch (const std::exception&) {
                return false;
            }
        }
        
        std::string getFormattedErrorMessages() const {
            return "JSON parsing failed";
        }
    };
}

