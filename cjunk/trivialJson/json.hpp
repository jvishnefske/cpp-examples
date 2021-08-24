#include <variant>
#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>


struct JsonNode{
    using Object = std::variant<std::string, int64_t, double,std::vector< JsonNode > >;
    Object object;
    std::string serialize();
};

template<typename Integer, std::enable_if_t< std::is_integral<Integer>::value, bool> = true>
JsonNode generator(Integer i){
    JsonNode j;
    j.object = static_cast<int64_t>(i);
    return j;
}

template<typename T,std::enable_if_t< std::is_convertible<T, JsonNode::Object>::value, bool> = true >
JsonNode generator(T thingy){
    JsonNode j;
    j.object = thingy;
    return j;
}

template<typename ...Args>
JsonNode generator(Args const & ... thingies){
    JsonNode j;
    std::vector<JsonNode> container{ {generator(thingies) ...} };
    j.object = container;
    return j;
}