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


    // JsonNode(const JsonNode& arg){
    //     std::visit([this](auto&& arg){
    //         using T = std::decay_t<decltype(arg)>;
    //         if constexpr(std::is_same_v<T, std::monostate>){
    //             // do nothing
    //         }else if constexpr(std::is_same_v<T, bool>){
    //             new (&storage) bool(arg);
    //         }else if constexpr(std::is_same_v<T, long>){
    //             new (&storage) long(arg);
    //         }else if constexpr(std::is_same_v<T, double>){
    //             new (&storage) double(arg);
    //         }else if constexpr(std::is_same_v<T, String>){
    //             new (&storage) String(new std::string(*arg));
    //         }else if constexpr(std::is_same_v<T, Map>){
    //             new (&storage) Map(new std::map<std::string,JsonNode>(*arg));
    //         }else if constexpr(std::is_same_v<T, List>){
    //             new (&storage) List(new std::list<JsonNode>(*arg));
    //         }
    //     }, arg.storage);
    // }
    //     // copy assignment
    // JsonNode& operator=(const JsonNode& other){
    //     if(this == &other){
    //         return *this;
    //     }
    //     // deep copy storage.
    //     std::visit([this](auto&& arg){
    //         using T = std::decay_t<decltype(arg)>;
    //         if constexpr(std::is_same_v<T, std::monostate>){
    //             // do nothing
    //         }else if constexpr(std::is_same_v<T, bool>){
    //             new (&storage) bool(arg);
    //         }else if constexpr(std::is_same_v<T, long>){
    //             new (&storage) long(arg);
    //         }else if constexpr(std::is_same_v<T, double>){
    //             new (&storage) double(arg);
    //         }else if constexpr(std::is_same_v<T, String>){
    //             new (&storage) String(new std::string(*arg));
    //         }else if constexpr(std::is_same_v<T, Map>){
    //             new (&storage) Map(new std::map<int,int>(*arg));
    //         }else if constexpr(std::is_same_v<T, List>){
    //             new (&storage) List(new std::list<int>(*arg));
    //         }
    //     }, other.storage);
    //     return *this;
    // }


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
    
    // this one is non trivial
    //auto myMap = std::map<std::string,JsonNode>{{"a",JsonNode(1)},{"b",JsonNode(2)}};
    //JsonNode node6(myMap);
    
    //JsonNode node7(std::vector<int>{1,2,3});

    //copy constructor
    //JsonNode node8(node3);
} 
} // namespace