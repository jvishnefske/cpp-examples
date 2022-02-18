#include <variant>
#include <future>
#include <iterator>
#include <regex>
using Node = std::variant<std::vector<Node>, long, bool, std::string, std::map<std::string, Node> >;

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
    parseJson(r"{"apple":[1,2.0,4], "pear": "fruit2", "address": true}");
}
Node parseJson(std::string const& s) {
    std::regex re("(\\{)|(\\})|(\\[)|(\\])|(\\,)|(\\:)|(\\d+)|(\\w+)|(\\s+)|(\\W+)");
    std::smatch m;
    std::regex_search(s, m, re);
    std::vector<Node> nodes;
    for (auto& x : m) {
        if (x.matched) {
            if (std::regex_match(x.str(), std::regex("\\{|\\}"))) {
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
// rewrite Node as a class
class Node {
public:
    Node(std::vector<Node> nodes) : nodes_(std::move(nodes)) {}
    Node(long long n) : nodes_(n) {}
    Node(bool b) : nodes_(b) {}
    Node(std::string s) : nodes_(s) {}
    Node(std::map<std::string, Node> m) : nodes_(m) {}
    Node(std::variant<std::vector<Node>, long, bool, std::string, std::map<std::string, Node> > v) : nodes_(std::move(v)) {}
    Node(Node&&) = default;
    Node& operator=(Node&&) = default;
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node const&) = delete;
    Node& operator=(Node const&) = delete;
    ~Node() = default;
    Node& operator[](size_t i) { return nodes_[i]; }
    Node& operator[](std::string const& s) { return nodes_[s]; }
    Node& operator[](char const* s) { return nodes_[s]; }
    Node& operator[](std::initializer_list<std::string> const& s) { return nodes_[s]; }
};
// std::visit node to serialize json to stream
template <typename T>
struct serialize {
    template <typename U>
    void operator()(U&& u) {
        std::visit([&](auto&& x) {
            std::cout << x;
        }, u);
    }
};
template <typename T>
void serialize(T&& t) {
    std::visit(serialize<T>{}, std::forward<T>(t));
}
// std::visit node to serialize json to stream
template <typename T>
struct serialize {
    template <typename U>
    void operator()(U&& u) {
        std::visit([&](auto&& x) {
            std::cout << x;
        }, u);
    }
};
template <typename T>
void serialize(T&& t) {
    std::visit(serialize<T>{}, std::forward<T>(t));
}
