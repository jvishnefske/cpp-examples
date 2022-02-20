struct Node {
    using SmallString = std::array<char, 8>;
//    using BigString = std::unique_ptr<std::string>;
//    using Object = std::map<std::string, Node>;
    //using ObjectPtr = std::unique_ptr<std::map<std::string, Node>>;
    using List = std::vector<Node>;
    using ListPtr = std::shared_ptr<List>;
    // reorder to put something sane for trivial construction.
    using Storage = std::variant<SmallString, int64_t, double, ListPtr>;

    template<typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
    explicit Node(const Integer i): _storage{static_cast<int64_t>(i)} {}

    template<typename T, std::enable_if_t<std::is_convertible_v<T, Storage>, bool> = true>
    explicit Node(const T obj): _storage(obj) {}

    // only needed for small strings since char* is not convertable to SmallString
    template<typename T, std::enable_if_t<std::is_convertible_v<T, std::string>, bool> = true>
    explicit Node(T obj):_storage(generator(obj)._storage) {}

    template<typename ...Args
            // check if each type is constructable
            //,std::enable_if_t<(... && std::is_constructible_v<Node, Args>)> = true
    >
    explicit Node(Args const &... args) {
        std::cout << "variadic template constructor" << std::endl;
        //auto generator = [](auto arg){return Node(arg);};
        std::vector<Node> container{generator(args) ...};
        // (  container.push_back(args)  ... );
        // todo: is there an unnecessary copy here??
        _storage = std::make_shared<std::vector<Node> >(container);
    }

    explicit Node() = default;

    /**
     *
     * @tparam Visitor
     * @param v
     * @return
     */
    template<typename Visitor>
    auto visit(Visitor v) {
        static_assert(std::is_move_constructible_v<Visitor>, "move construdtable visotor");
        static_assert(std::is_class_v<Visitor>, "visitor should be class");
//        static_assert(std::is_trivial_v<Visitor>, "visitor should be trivial");
        return std::visit(v, _storage);
    }

    std::string serialize();

    Storage _storage;

protected:


    template<typename T>
    struct IsStringLiteral :
            std::is_same<
                    T,
                    std::add_lvalue_reference_t<const char[std::extent_v<std::remove_reference_t<T>>]>
            > {
    };

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

    // small string
    template<typename T, std::enable_if_t<std::is_convertible_v<T, std::string>, bool> = true>
    Node generator(T thingy) {
        std::clog << "constructing small string" << std::endl;
        Node j;
        // if the null from c_str below is not used we may need to explicitly terminate with null.
        SmallString s;
        static_assert(sizeof(thingy) <= sizeof(SmallString), "string is not small");
        const auto temp = std::string(thingy);
        // add one to string size for null terminator
        const auto len = std::min(temp.size() + 1, sizeof(thingy));
        std::copy_n(temp.c_str(), len, s.begin());
        j._storage = s;
        return j;
    }
};
