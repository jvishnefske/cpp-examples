#pragma once
#include <variant>
#include <iosfwd>
#include <string>
#include <vector>
#include <cctype>


struct Node{
    using Storage = std::variant<std::string, int64_t, double, std::vector<Node> >;

//    template<typename Integer, std::enable_if_t< std::is_integral_v<Integer>, bool> = true>
//    explicit Node(const Integer i): _storage{static_cast<int64_t>(i)} {}

    template<typename T, std::enable_if_t< std::is_convertible_v<T, Storage>, bool> = true >
    explicit Node(const T obj): _storage(obj) {}

    template<typename ...Args
    // check if each type is constructable
             //,std::enable_if_t<(... && std::is_constructible_v<Node, Args>)> = true
             >
    explicit Node(Args  const & ... args){
        //auto generator = [](auto arg){return Node(arg);};
        std::vector<Node> container{ generator(args) ...  };
        //(  container.push_back(args)  ... );
        _storage = container;
    }

    explicit Node() = default;

    /**
     *
     * @tparam Visitor
     * @param v
     * @return
     */
    template<typename Visitor>
    auto visit(Visitor v){
        return std::visit(v, _storage);
    }
    std::string serialize();
protected:
    Storage _storage;

    /**
     * These generate functions were included simply because the MSVC++ gave me compile errors
     * when attempting to add parameter pack expansion to a Node constructor.
     * @tparam Integer
     * @param i
     * @return
     */
    template<typename Integer, std::enable_if_t< std::is_integral<Integer>::value, bool> = true>
    Node generator(Integer i){
        Node j;
        j._storage = static_cast<int64_t>(i);
        return j;
    }

    template<typename T,std::enable_if_t< std::is_convertible<T, Node::Storage >::value, bool> = true >
    Node generator(T thingy){
        Node j;
        j._storage = thingy;
        return j;
    }
};

// how to differentiate serialization methods
std::ostream& operator<<(std::ostream &os, const Node &obj);

