#pragma once
#include <variant>
#include <iosfwd>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <memory>
#include <array>

struct Node {
    using SmallString = std::array<char, 8>;
    using List = std::vector<Node>;
    using ListPtr = std::shared_ptr<List>;
    // reorder to put something sane for trivial construction.
    using Storage = std::variant<SmallString, int64_t, double, ListPtr>;

    template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
    constexpr explicit Node(const Integer i): _storage{static_cast<int64_t>(i)} {}

    template<typename T, std::enable_if_t<std::is_convertible_v<T, Storage>, bool> = true>
    constexpr explicit Node(const T obj): _storage(obj) {}
    // only needed for small strings since char* is not convertable to SmallString
//    template<typename T, std::enable_if_t<std::is_convertible_v<T, std::string>, bool> = true>
//    explicit Node(T obj)
//        //: _storage(generator(obj)._storage)
//        {
//            _storage.template emplace<std::array>({})
//        }


// todo try to make this work
//    constexpr Node(const Node&) = default;
//    constexpr Node(Node&&) = default;
//    constexpr Node& operator=(const Node&) = default;
//    constexpr Node& operator=(Node&&) = default;
//    constexpr bool operator==(const Node& rhs) const {
//        return _storage == rhs._storage;
//    }
    constexpr bool operator!=(const Node &rhs) const {
        return _storage != rhs._storage;
    }

    // declare  JsonVisitor as friend so it can access private members
    template<typename Visitor>
    friend auto visit(Visitor &visitor, const Node &node) -> decltype(visitor(node)) {
        return visitor(node);
    }

    constexpr size_t length(const char *s) {
        return *s ? length(s + 1) + 1 : 1;
    }

    explicit Node(const char *s) {
        SmallString d{};
        std::copy_n(s, std::min(length(s), sizeof(SmallString)), d.data());
        _storage = d;
    }

    template<typename ...Args>
    explicit Node(Args const &... args){
        //auto generator = [](auto arg){return Node(arg);};
        std::vector<Node> container{ generator(args) ...  };
        //(  container.push_back(args)  ... );
        _storage = container;
    }

    Node() = default;

    /**
     *
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

    template<typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
    Node generator(Integer i) {
        Node j;
        j._storage = static_cast<int64_t>(i);
        return j;
    }

    template<typename T, std::enable_if_t<std::is_convertible<T, Node::Storage>::value, bool> = true>
    Node generator(T thingy) {
        Node j;
        j._storage = thingy;
        return j;
    }
};

// how to differentiate serialization methods
std::ostream& operator<<(std::ostream &os, const Node &obj);

