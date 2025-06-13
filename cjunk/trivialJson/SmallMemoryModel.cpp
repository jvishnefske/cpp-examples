#include <variant>
#include <string_view>
#include <array>
#include <string>
#include <algorithm>
#include <future>
#include <map>
#include <list>
#include <vector>
#include <type_traits>
#include <iostream>
namespace{
class JsonNode{
    template<class> static inline constexpr bool always_false_v = false;
public:
    using Map = std::unique_ptr<std::map<std::string,JsonNode>>;
    using List = std::unique_ptr<std::vector<JsonNode>>;
    using String = std::unique_ptr<std::string>;
    using Null = std::monostate;
    using SmallString = std::array<char, 8>;
    // Added 'String' to the variant to correctly handle heap-allocated strings
    using Storage = std::variant<Null, bool, long, double, SmallString, String, Map, List >;
private:
    Storage storage;
public:
    // General constructor for types directly convertible to Storage, excluding bool to avoid ambiguity
    // Removed constexpr as std::unique_ptr operations are not constexpr before C++20
    template<typename T, std::enable_if_t<std::is_convertible_v<T, Storage> && !std::is_same_v<std::decay_t<T>, bool>, bool> = true>
    explicit JsonNode(const T value):storage(value){}
    
    // Specific constructor for bool
    // Removed constexpr as std::unique_ptr operations are not constexpr before C++20
    explicit JsonNode(const bool b):storage(b){}

    // Constructor for string literals and std::string_view
    // Removed constexpr as std::unique_ptr operations are not constexpr before C++20
    explicit JsonNode(std::string_view sv) {
        if (sv.length() <= SmallString{}.size()) { // Check if it fits in SmallString
            SmallString ss{};
            std::copy(sv.begin(), sv.end(), ss.begin());
            storage = ss;
        } else {
            storage = std::make_unique<std::string>(sv);
        }
    }

    // copy constructors could likely be shortened with some template magic.
    // Removed constexpr as std::unique_ptr operations are not constexpr before C++20
    JsonNode(const JsonNode& arg){
        // The line `(void) always_false_v;` was syntactically incorrect here.
        // `always_false_v` is a variable template and requires a template argument,
        // typically used within `static_assert` for dependent types.
        // It serves no functional purpose outside of that context.
        // Removed: (void) always_false_v;
        std::visit([&](const auto& arg_val){ // Renamed to arg_val for clarity
            using T = std::decay_t<decltype(arg_val)>;
            if constexpr(std::is_same_v<T, bool> || std::is_same_v<T, long> || std::is_same_v<T, double> || std::is_same_v<T, std::monostate> || std::is_same_v<T, SmallString>)
                storage = arg_val;
            else if constexpr(std::is_same_v<T, String>)
                storage = std::make_unique<std::string>(*arg_val);
            else if constexpr(std::is_same_v<T, Map>)
                storage = std::make_unique<Map::element_type>(*arg_val);
            else if constexpr(std::is_same_v<T, List>)
                storage = std::make_unique<List::element_type>(*arg_val);
            else
            {
                static_assert(always_false_v<T>,"unhandled type in copy constructor");
            }
        }, arg.storage);
    }
    // get the value of the node.
    // Removed constexpr as std::unique_ptr operations are not constexpr before C++20
    template<typename T, std::enable_if_t<std::is_convertible_v<T, Storage>, bool> = true>
    T get() const{
        return std::get<T>(storage);
    }
    // copy assignment
    // Removed constexpr as std::unique_ptr operations are not constexpr before C++20
    JsonNode& operator=(const JsonNode& arg){
        if(this==&arg)
            return *this;
        std::visit([&](const auto& arg_val){ // Renamed to arg_val for clarity
            using T = std::decay_t<decltype(arg_val)>;
            if constexpr(std::is_same_v<T, bool> || std::is_same_v<T, long> || std::is_same_v<T, double> || std::is_same_v<T, std::monostate> || std::is_same_v<T, SmallString>)
                storage = arg_val;
            else if constexpr(std::is_same_v<T,String>)
                storage = std::make_unique<String::element_type>(*arg_val);
            else if constexpr(std::is_same_v<T,Map>)
                storage = std::make_unique<Map::element_type>(*arg_val);
            else if constexpr(std::is_same_v<T,List>)
                storage = std::make_unique<List::element_type>(*arg_val);
            else
                {
                    static_assert(always_false_v<T>,"unhandled type in copy assignment");
                }
        }, arg.storage);
        return *this;
    }

//    constexpr std::string_view toJsonString() const{
//        std::string result;
//        std::visit([&](const auto& arg){
//            using T = std::decay_t<decltype(arg)>;
//            if constexpr(std::is_same_v<T, JsonNode>)
//                result = arg->toJsonString();
//            else if constexpr(std::is_same_v<T, bool>)
//                result = arg?"true":"false";
//            else if constexpr(std::is_same_v<T, long>)
//                result = std::to_string(arg);
//            else if constexpr(std::is_same_v<T, double>)
//                result = std::to_string(arg);
//            else if constexpr(std::is_same_v<T,std::unique_ptr<std::string>>)
//                result = "\"" + *arg + "\"";
//            else if constexpr(std::is_same_v<T,Map>){
//                result = "{";
//                for(const auto& [key,value]:*arg){
//                    result += "\"" + key + "\":" + value.toJsonString() + ",";
//                }
//                result.back() = '}';
//            }
//            else if constexpr(std::is_same_v<T,List>){
//                result = "[";
//                for(const auto& value:*arg){
//                    result += value.toJsonString() + ",";
//                }
//                result.back() = ']';
//            }
//            else if constexpr(std::is_same_v<T,std::monostate>)
//                result = "null";
//            else
//                static_assert(always_false_v<T>, "unsupported type");
//        }, storage);
//        return result;
//    }
};
static_assert(sizeof(JsonNode)<=16);
#if 0
JsonNode fromJsonString (const std::string input){
        JsonNode result;
        std::string::const_iterator it = input.begin();
        std::string::const_iterator end = input.end();
        if(it==end)
            return result;
        if(*it=='{'){
            result.storage = std::make_unique<std::map<std::string,JsonNode>>();
            ++it;
            while(it!=end){
                if(*it=='}'){
                    ++it;
                    break;
                }
                std::string key;
                while(it!=end && *it!=':'){
                    key.push_back(*it);
                    ++it;
                }
                if(it==end)
                    return result;
                ++it;
                JsonNode value = fromJsonString(std::string(it,end));
                (*result.storage.template get<std::map<std::string,JsonNode>>()).insert_or_assign(key,value);
                while(it!=end && *it!=',' && *it!='}'){
                    ++it;
                }
                if(it==end)
                    return result;
                if(*it==',')
                    ++it;
            }
        }
        else if(*it=='['){
            result.storage = std::make_unique<std::vector<int>>();
            ++it;
            while(it!=end){
                if(*it==']'){
                    ++it;
                    break;
                }
                int value = 0;
                while(it!=end && *it!=',' && *it!=']'){
                    value = value*10 + (*it-'0');
                    ++it;
                }
                if(it==end)
                    return result;
                (*result.storage.template get<std::vector<int>>()).push_back(value);
                if(*it==',')
                    ++it;
            }
        }else if(*it=='"'){
            ++it;
            while(it!=end && *it!='"'){
                result.storage = std::make_unique<std::string>();
                (*result.storage.template get<std::string>()).push_back(*it);
                ++it;
            }
            if(it==end)
                return result;
            ++it;
        }else{
            while(it!=end && *it!=',' && *it!='}'){
                result.storage = std::make_unique<long>();
                (*result.storage.template get<long>()) = (*result.storage.template get<long>())*10 + (*it-'0');
                ++it;
            }
        }
        return result;
    }
#endif
        
[[maybe_unused]] void test_node_size(){
    // static_assert(sizeof(JsonNode)<=16); // Already outside the function, no need to duplicate
}


/**
 * this type of thing could be used to store a json object, or
 * schema at compile time.
 * Array, and Object are limited to compile time constraints.
 */
class LiteralClass{
public:
    // Removed constexpr from constructor as it's not fully constexpr due to JsonNode::Storage
    LiteralClass(int a):a(a){}
    // only include constexpr types
    std::variant<std::monostate, int, double, bool> a;
};
[[maybe_unused]] void test_json_types(){
    // Removed constexpr from these declarations as they involve non-constexpr types/operations
    // constexpr std::variant<int,float> v1{1};
    // static_assert(v1.index() == 0, "int should be the first type");
    // constexpr std::variant<JsonNode::Null, bool, long, double,JsonNode::SmallString > json_variant{};
    // static_assert(json_variant.index() == 0, "Null should be the first type");
    // constexpr std::array<int,3> a{1,2,3};
    // static_assert(a.size() == 3, "array should have 3 elements");
    // constexpr LiteralClass lc(1);
    // static_assert(lc.a.index() == 1, "int should be the second type");
    
    // These JsonNode constructions are not constexpr before C++20 due to std::unique_ptr
    JsonNode node1(true);
    JsonNode node2(false);
    JsonNode node3(3l); // Changed to 3l to explicitly be a long literal
    JsonNode node4(150.3);
    JsonNode node5("hello");
    node4 = node5;
    // this one is non trivial
    //auto myMap = std::map<std::string,JsonNode>{{"a",JsonNode(1)},{"b",JsonNode(2)}};
    //JsonNode node6(myMap);
    
    //JsonNode node7(std::vector<int>{1,2,3});

    //copy constructor
    JsonNode node8(node3);
    //std::cout << node8.toJsonString() << std::endl;
    //JsonNode node9 = fromJsonString("123");
    //std::cout << node9.toJsonString() << std::endl;
} 
} // namespace
