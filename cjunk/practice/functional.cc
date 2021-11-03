#include <algorithm>
#include <array>
#include <future>

template<typename T, int size>
class ImmutableList {
public:
    ImmutableList(std::initializer_list<int> list) : _list(list) {}

    T operator[](int i) const {
        return _list.at(i);
    }

private:
    std::array<T, size> _list;
};

// map class returning Immutable List
template<typename From, int size, typename To>
class ImmutableMap {
private:
    ImmutableList<To, size> _from;
    std::function<To(From)> f;
public:
    ImmutableMap(ImmutableList<From,size>, std::function<To(From)>) : _map(list, f) {}

    To operator[](int i) const {
        return f(_from[i]);
    }
};

// list iterator
template<typename T, int size>
class ImmutableListIterator {
public:
    ImmutableListIterator(const ImmutableList<T, size> list):_list(list){}
    ImmutableListIterator(std::initializer_list<T> list) : _list(list) {}

    T operator*() const {
        return _list.at(i);
    }

    ImmutableListIterator& operator++() {
        ++i;
        return *this;
    }

    bool operator==(const ImmutableListIterator& rhs) const {
        return i == rhs.i;
    }

    bool operator!=(const ImmutableListIterator& rhs) const {
        return i != rhs.i;
    }

private:
    std::array<T, size> _list;
    int i = 0;
};

// list iterator
template<typename T, int size>
class ImmutableListReverseIterator {
public:
    ImmutableListReverseIterator(std::initializer_list<int> list) : _list(list) {}

    T operator*() const {
        return _list.at(i);
    }

    ImmutableListReverseIterator& operator++() {
        --i;
        return *this;
    }

    bool operator==(const ImmutableListReverseIterator& rhs) const {
        return i == rhs.i;
    }

    bool operator!=(const ImmutableListReverseIterator& rhs) const {
        return i != rhs.i;
    }

private:
    std::array<T, size> _list;
    int i = size - 1;
};

// filter iterator given boolean function, and iterator
template<typename T, int size, typename Predicate>
class Filter {
public:
    Filter(const ImmutableList<T, size> list, Predicate p) : _list(list), _predicate(p) {}

    T operator*() const {
        return _list.at(i);
    }

    Filter& operator++() {
        ++i;
        return *this;
    }

    bool operator==(const Filter& rhs) const {
        return i == rhs.i;
    }

    bool operator!=(const Filter& rhs) const {
        return i != rhs.i;
    }

private:
    std::array<T, size> _list;
    int i = 0;
    Predicate _predicate;
};

// filter iterator given boolean function, and iterator
template<typename T, int size, typename Predicate>
class FilterReverse {
public:
    FilterReverse(std::initializer_list<int> list, Predicate p) : _list(list), _predicate(p) {}

    T operator*() const {
        return _list.at(i);
    }

    FilterReverse& operator++() {
        --i;
        return *this;
    }

    bool operator==(const FilterReverse& rhs) const {
        return i == rhs.i;
    }

    bool operator!=(const FilterReverse& rhs) const {
        return i != rhs.i;
    }

private:
    std::array<T, size> _list;
    int i = size - 1;
    Predicate _predicate;
};
void test_map() {
    ImmutableList<int, 5> list({1, 2, 3, 4, 5});
    ImmutableMap<int, 5, int> map(list, [](int i) { return i * 2; });

    for (int i = 0; i < 5; i++) {
        std::cout << map[i] << std::endl;
    }
}
void test_filter(){
    ImmutableList<int, 5> list({1, 2, 3, 4, 5});
    Filter<int, 5, std::function<bool(int)>> filter(list, [](int i) { return i % 2 == 0; });

    for (int i = 0; i < 5; i++) {
        std::cout << filter[i] << std::endl;
    }
}
void test_filter_reverse(){
    ImmutableList<int, 5> list({1, 2, 3, 4, 5});
    FilterReverse<int, 5, std::function<bool(int)>> filter(list, [](int i) { return i % 2 == 0; });

    for (int i = 0; i < 5; i++) {
        std::cout << filter[i] << std::endl;
    }
}
int main() {
    test_map();
    test_filter();
    test_filter_reverse();
}

