#include <json.hpp>
#include <algorithm>
#include <ostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>


// todo: add operator<< for each serialization type bson, json, etc.
// todo convert generators to trivial constructors
// todo move more includes into implementation: consider using a unique_ptr to variant, and fwd declaring variant.
// todo add json objects of type std::map<string, JsonNode>
// todo consider adding bson types where they make sense for other serialization methods decimal128 datetime in milliseconds
// todo make constexpr operator""json
// todo is it possble to use compile time schemas and create node.member which links to {"member": value}
struct JsonVisitor : Node {
    std::string operator()(const ListPtr o) {
        if (!o) {
            // for debugging
            return "nullptr!!!";
        }
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for (auto _o: *o) {
            if (first) {
                first = false;
            } else {
                oss << ",";
            }
            //oss << "todo 1";//_o.visit(*this);
            oss << std::visit(*this, _o._storage);
        }
        oss << "]";
        return oss.str();
    }

    std::string operator()(const std::string o) const {
        return std::string("\"") + o + std::string("\"");
    }

    std::string operator()(const Node::SmallString str) {
        std::cout << "CharArray" << typeid(str).name() << std::endl;

        // explicitly specify the string length since str is not always null terminated.
        // note that this will always make std::string::size always return 8 including any optional terminating characters.
        // c_str is used to truncate based on a null terminator which std::string provides.
        return std::string("\"") + std::string(str.data(), str.size()).c_str() + std::string("\"");
    }

    template<typename T>
    std::string operator()(T o) const {
        std::cout << "to_string" << std::endl;
        return std::to_string(o);
    }
};

auto operator<<(std::ostream &os, Node object) -> std::ostream& {
    os << object.serialize();
    return os;
}

std::string Node::serialize() {
    std::ostringstream oss;
    JsonVisitor v;
    //return visit(v);
    return std::visit(v, this->_storage);
}
