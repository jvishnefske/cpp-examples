#include <json.hpp>
#include <variant>
#include <algorithm>

// todo: add operator<< for each serialization type bson, json, etc.
// todo convert generators to trivial constructors
// todo move more includes into implementation: consider using a unique_ptr to variant, and fwd declaring variant.
// todo add json objects of type std::map<string, JsonNode>
// todo consider adding bson types where they make sense for other serialization methods decimal128 datetime in milliseconds
// todo make constexpr operator""json
// todo is it possble to use compile time schemas and create node.member which links to {"member": value}
struct JsonVisitor {
    std::string operator()  ( std::vector<JsonNode> o)const{
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for(auto _o: o){
            if (first){
                first = false;
            }else{
                oss << ",";
            }
            oss << std::visit(*this, _o.object);
        }
        oss << "]";
        return oss.str();
    }
    std::string operator()( std::string o)const {
        return std::string("\"")+o+std::string("\"");
    }
    template <typename T>
    std::string operator() ( T o) const {
        return std::to_string(o);
    }
};

std::string JsonNode::serialize(){
    JsonVisitor v;
    return std::visit(v, object);
}
