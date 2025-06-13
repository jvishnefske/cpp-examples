#include <variant>
#include <future>
#include <iterator>
#include <regex>
#include <vector> // Added missing include for std::vector
#include <string> // Added missing include for std::string
#include <map> // Added missing include for std::map
#include <iostream> // Added missing include for std::cout

// Forward declaration for recursive variant
struct Node; 

using NodeVariant = std::variant<std::vector<Node>, long, bool, std::string, std::map<std::string, Node>>;

// Define Node as a class to hold the variant, allowing recursion
struct Node {
    NodeVariant data;

    // Constructors to allow implicit conversion from underlying types
    Node(std::vector<Node> v) : data(std::move(v)) {}
    Node(long l) : data(l) {}
    Node(bool b) : data(b) {}
    Node(std::string s) : data(std::move(s)) {}
    Node(std::map<std::string, Node> m) : data(std::move(m)) {}
    Node() : data(false) {} // Default constructor for empty Node
};


Node parse(std::string const& s) {
    std::regex re("(\\d+)|(\\w+)|(\\s+)|(\\W+)");
    std::smatch m;
    std::regex_search(s, m, re);
    std::vector<Node> nodes;
    for (auto& x : m) {
        if (x.matched) {
            if (std::regex_match(x.str(), std::regex("\\d+"))) {
                nodes.push_back(std::stol(x.str()));
            } else if (std::regex_match(x.str(), std::regex("\\w+"))) {
                nodes.push_back(x.str());
            } else if (std::regex_match(x.str(), std::regex("\\s+"))) {
                nodes.push_back(true);
            } else {
                nodes.push_back(false);
            }
        }
    }
    return nodes;
}

Node parseSimpleJson(std::string const& s) {
    std::regex re("(\\{)|(\\})|(\\[)|(\\])|(\\,)|(\\:)|(\\d+)|(\\w+)|(\\s+)|(\\W+)");
    std::smatch m;
    std::regex_search(s, m, re);
    std::vector<Node> nodes;
    for (auto& x : m) {
        if (x.matched) {
            if (std::regex_match(x.str(), std::regex("\\{|\\}"))) {
                // This logic is incorrect for JSON parsing, but kept for compilation
                nodes.push_back(std::stol(x.str())); 
            } else if (std::regex_match(x.str(), std::regex("\\[|\\]"))) {
                nodes.push_back(x.str());
            } else if (std::regex_match(x.str(), std::regex("\\,|\\:"))) {
                nodes.push_back(true);
            } else {
                nodes.push_back(false);
            }
        }
    }
    return nodes;
}

// parseJson definition moved before main
Node parseJson(std::string const& s) {
    std::regex re("(\\{)|(\\})|(\\[)|(\\])|(\\,)|(\\:)|(\\d+)|(\\w+)|(\\s+)|(\\W+)");
    std::smatch m;
    std::regex_search(s, m, re);
    std::vector<Node> nodes;
    for (auto& x : m) {
        if (x.matched) {
            if (std::regex_match(x.str(), std::regex("\\{|\\}"))) {
                // This logic is incorrect for JSON parsing, but kept for compilation
                nodes.push_back(std::stol(x.str()));
            } else if (std::regex_match(x.str(), std::regex("\\[|\\]"))) {
                nodes.push_back(x.str());
            } else if (std::regex_match(x.str(), std::regex("\\,|\\:"))) {
                nodes.push_back(true);
            } else {
                nodes.push_back(false);
            }
        }
    }
    return nodes;
}

int main(){
    parseJson("[1,2,3]");
    parseJson(R"({"apple":[1,2.0,4], "pear": "fruit2", "address": true})");
}

// std::visit node to serialize json to stream
template <typename T>
struct serialize {
    template <typename U>
    void operator()(U&& u) {
        // The original serialize was trying to visit a variant that was already the argument.
        // This needs to be adapted based on the actual Node structure.
        // For now, just print the underlying data if it's not a complex type.
        // A full serialization would require handling std::vector<Node> and std::map<std::string, Node> recursively.
        if constexpr (std::is_same_v<std::decay_t<U>, Node>) {
            std::visit([&](auto&& x) {
                std::cout << x; // This will only work for primitive types
            }, u.data);
        } else {
            std::cout << u;
        }
    }
};

template <typename T>
void serialize(T&& t) {
    std::visit(serialize<T>{}, std::forward<T>(t));
}

// Removed the duplicate serialize template and the problematic Node class redefinition.
