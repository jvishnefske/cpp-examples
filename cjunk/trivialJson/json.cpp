#include <json.hpp>
#include <algorithm>
#include <ostream>
#include <sstream>

// todo: add operator<< for each serialization type bson, json, etc.
// todo convert generators to trivial constructors
// todo move more includes into implementation: consider using a unique_ptr to variant, and fwd declaring variant.
// todo add json objects of type std::map<string, JsonNode>
// todo consider adding bson types where they make sense for other serialization methods decimal128 datetime in milliseconds
// todo make constexpr operator""json
// todo is it possble to use compile time schemas and create node.member which links to {"member": value}
struct JsonVisitor: Node {
    std::string operator()  ( const std::vector<Node> o)const{
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for(auto _o: o){
            if (first){
                first = false;
            }else{
                oss << ",";
            }
            oss << _o.visit(*this);
            //oss << std::visit(*this, _o._storage);
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

std::ostream& operator<<(std::ostream &os, Node object) {
    os << object.serialize();
    return os;
}

std::string Node::serialize() {
    std::ostringstream oss;
    JsonVisitor v;
    return visit(v);
}
