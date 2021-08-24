#include <variant>
#include <iosfwd>
#include <string>
#include <vector>
#include <cctype>
#include <functional>


struct Node{
    using Storage = std::variant<std::string, int64_t, double, std::vector<Node> >;

    template<typename T, std::enable_if_t< std::is_convertible_v<T, Storage>, bool> = true >
    explicit Node(T obj): _storage(obj) {}

    template<typename Integer, std::enable_if_t< std::is_integral_v<Integer>, bool> = true>
    explicit Node(Integer i): _storage{static_cast<int64_t>(i)} {}


    template<typename ...Args
    // check if each type is constructable
             ,std::enable_if_t<(... && std::is_constructible_v<Node, Args>)> = true
             >
    explicit Node(Args ... args){
        auto generator = [](auto arg){return Node(arg);};
        std::vector<Node> container{ {Node(args) ...} };
        _storage = container;
    }

    explicit Node() = default;

    /**
     * consider using auto parameter pack deduction to reduce template
     * @tparam Args
     * @param thingies
     * todo: add concept, or type qualifier
     */
//    template<typename ...Args>
//    Node(Args const & ... thingies);
    //
    template<typename Visitor>
    auto visit(Visitor v){
        return std::visit(v, _storage);
    }
    std::string serialize();
protected:
    Storage _storage;
};

// how to differentiate serialization methods
std::ostream& operator<<(std::ostream &os, const Node &obj);


//struct JsonNode{
//    using Object = std::variant<std::string, int64_t, double,std::vector< JsonNode > >;
//    Object object;
//    std::string serialize();
//};


