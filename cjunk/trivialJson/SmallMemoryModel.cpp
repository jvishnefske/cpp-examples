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


namespace{   
    template<class> inline constexpr bool always_false_v = false; 
class JsonNode{
    using Node = std::unique_ptr<JsonNode>;
    using Map = std::unique_ptr<std::map<std::string,JsonNode>>;
    using List = std::unique_ptr<std::vector<int>>;
    using String = std::unique_ptr<std::string>;
    using Null = std::monostate;
    using SmallString = std::array<char, 8>;
    using Storage = std::variant<Null, bool, long, double,String, Map, List >;
    Storage storage;
    //int64_t i;
        public:
    template<typename T, std::enable_if_t<std::is_convertible_v<T, Storage>, bool> = true>
    explicit JsonNode(const T value):storage(value){}
    explicit JsonNode(const bool b):storage(b){}
    // copy constructors could likily be shotened with some template magic.
     JsonNode(const JsonNode& arg){
        std::visit([&](const auto& arg){
            using T = std::decay_t<decltype(arg)>;
            if constexpr(std::is_same_v<T, JsonNode>)
                storage = arg.storage;
            else if constexpr(std::is_same_v<T, bool>)
                storage = arg;
            else if constexpr(std::is_same_v<T, long>)
                storage = arg;
            else if constexpr(std::is_same_v<T, double>)
                storage = arg;
            else if constexpr(std::is_same_v<T,std::unique_ptr<std::string>>)
                storage = std::make_unique<std::string>(*arg);
            else if constexpr(std::is_same_v<T,std::unique_ptr<std::map<std::string,JsonNode>>>)
                storage = std::make_unique<std::map<std::string,JsonNode>>(*arg);
            else if constexpr(std::is_same_v<T,std::unique_ptr<std::vector<int>>>)
                storage = std::make_unique<std::vector<int>>(*arg);
            else if constexpr(std::is_same_v<T,std::monostate>)
                storage = arg;
            else
                static_assert(always_false_v<T>, "unsupported type");
        }, arg.storage);
    }
    // copy assignment
    JsonNode& operator=(const JsonNode& arg){
        if(this==&arg)
            return *this;
        std::visit([&](const auto& arg){
            using T = std::decay_t<decltype(arg)>;
            if constexpr(std::is_same_v<T, JsonNode>)
                storage = arg.storage;
            else if constexpr(std::is_same_v<T, bool>)
                storage = arg;
            else if constexpr(std::is_same_v<T, long>)
                storage = arg;
            else if constexpr(std::is_same_v<T, double>)
                storage = arg;
            else if constexpr(std::is_same_v<T,std::unique_ptr<std::string>>)
                storage = std::make_unique<std::string>(*arg);
            else if constexpr(std::is_same_v<T,std::unique_ptr<std::map<std::string,JsonNode>>>)
                storage = std::make_unique<std::map<std::string,JsonNode>>(*arg);
            else if constexpr(std::is_same_v<T,std::unique_ptr<std::vector<int>>>)
                storage = std::make_unique<std::vector<int>>(*arg);
            else if constexpr(std::is_same_v<T,std::monostate>)
                storage = arg;
            else
                static_assert(always_false_v<T>, "unsupported type");
        }, arg.storage);
        return *this;
    }
};
void test_node_size(){
    static_assert(sizeof(JsonNode)<=16);
}

void test_json_types(){
    JsonNode node1(true);
    JsonNode node2(false);
    JsonNode node3(3);
    JsonNode node4(150.3);
    JsonNode node5("hello");
    node4 = node5;
    // this one is non trivial
    //auto myMap = std::map<std::string,JsonNode>{{"a",JsonNode(1)},{"b",JsonNode(2)}};
    //JsonNode node6(myMap);
    
    //JsonNode node7(std::vector<int>{1,2,3});

    //copy constructor
    JsonNode node8(node3);
} 
} // namespace