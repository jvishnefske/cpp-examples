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
    std::string operator()(const List o) {
#if 0
        if(!o){
            // for debugging
            throw std::runtime_error("nullptr in serialization helper operator()");
            return "nullptr!!!";
        }
#endif
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for (auto _o: o) {
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

    std::string operator()(std::string o) const {
        return std::string("\"") + o + std::string("\"");
    }


    //, std::enable_if_t<std::is_same_v<Node::SmallString, CharArray>, bool> = true
    template<typename CharArray, std::enable_if_t<std::is_same_v<Node::SmallString, CharArray>, bool> = true>
    std::string operator()(CharArray str) {
        std::cout << "CharArray" << typeid(str).name() << std::endl;

        // explicitly specify the string length since str is not always null terminated.
        // note that this will always make std::string::size always return 8 including any optional terminating characters.
        return std::string("\"") + std::string(str.data(), str.size()).c_str() + std::string("\"");
    }


    template<typename T, std::enable_if_t<std::is_constructible_v<std::string, T>, bool> = true>
    std::string operator()(T obj) {
        std::cout << "constructable" << typeid(obj).name() << std::endl;
        return std::string(obj);
    }

    template<typename T>
    std::string operator()(T o) const {
        std::cout << "to_string" << std::endl;
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
    //return visit(v);
    return std::visit(v, this->_storage);
}
