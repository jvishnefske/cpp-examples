#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <vector>
#include <numeric>
#include <iterator>

// Simple functional programming utilities
template<typename T, size_t N>
class ImmutableArray {
private:
    std::array<T, N> data_;
    
public:
    constexpr ImmutableArray() = default;
    
    constexpr ImmutableArray(std::initializer_list<T> init) {
        std::copy(init.begin(), init.end(), data_.begin());
    }
    
    constexpr const T& operator[](size_t index) const {
        return data_[index];
    }
    
    constexpr size_t size() const { return N; }
    
    constexpr auto begin() const { return data_.begin(); }
    constexpr auto end() const { return data_.end(); }
    
    // Map function - applies function to each element
    template<typename F>
    auto map(F func) const -> ImmutableArray<decltype(func(data_[0])), N> {
        ImmutableArray<decltype(func(data_[0])), N> result;
        std::transform(begin(), end(), result.data_.begin(), func);
        return result;
    }
    
    // Filter function - creates vector (size unknown at compile time)
    template<typename Predicate>
    std::vector<T> filter(Predicate pred) const {
        std::vector<T> result;
        std::copy_if(begin(), end(), std::back_inserter(result), pred);
        return result;
    }
    
    // Reduce function
    template<typename F>
    T reduce(F func, T initial = T{}) const {
        return std::accumulate(begin(), end(), initial, func);
    }
};

// Higher-order functions for vectors
namespace functional {
    template<typename Container, typename F>
    auto map(const Container& container, F func) {
        using ValueType = decltype(func(*container.begin()));
        std::vector<ValueType> result;
        result.reserve(container.size());
        std::transform(container.begin(), container.end(), 
                      std::back_inserter(result), func);
        return result;
    }
    
    template<typename Container, typename Predicate>
    auto filter(const Container& container, Predicate pred) {
        using ValueType = typename Container::value_type;
        std::vector<ValueType> result;
        std::copy_if(container.begin(), container.end(), 
                    std::back_inserter(result), pred);
        return result;
    }
    
    template<typename Container, typename F, typename T>
    T reduce(const Container& container, F func, T initial) {
        return std::accumulate(container.begin(), container.end(), initial, func);
    }
}

// Compose functions
template<typename F, typename G>
auto compose(F f, G g) {
    return [f, g](auto x) { return f(g(x)); };
}

// Curry function (partial application)
template<typename F>
auto curry(F f) {
    return [f](auto x) {
        return [f, x](auto y) {
            return f(x, y);
        };
    };
}

void test_immutable_array() {
    std::cout << "=== Testing ImmutableArray ===\n";
    
    // Create immutable array
    ImmutableArray<int, 5> numbers{1, 2, 3, 4, 5};
    
    // Map: square each number
    auto squares = numbers.map([](int x) { return x * x; });
    std::cout << "Squares: ";
    for (const auto& sq : squares) {
        std::cout << sq << " ";
    }
    std::cout << "\n";
    
    // Filter: even numbers only
    auto evens = numbers.filter([](int x) { return x % 2 == 0; });
    std::cout << "Evens: ";
    for (const auto& even : evens) {
        std::cout << even << " ";
    }
    std::cout << "\n";
    
    // Reduce: sum all numbers
    int sum = numbers.reduce([](int acc, int x) { return acc + x; }, 0);
    std::cout << "Sum: " << sum << "\n";
}

void test_functional_utilities() {
    std::cout << "\n=== Testing Functional Utilities ===\n";
    
    std::vector<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Map: double each number
    auto doubled = functional::map(numbers, [](int x) { return x * 2; });
    std::cout << "Doubled: ";
    for (const auto& d : doubled) {
        std::cout << d << " ";
    }
    std::cout << "\n";
    
    // Filter: numbers greater than 5
    auto large = functional::filter(numbers, [](int x) { return x > 5; });
    std::cout << "Greater than 5: ";
    for (const auto& l : large) {
        std::cout << l << " ";
    }
    std::cout << "\n";
    
    // Reduce: product of all numbers
    int product = functional::reduce(numbers, [](int acc, int x) { return acc * x; }, 1);
    std::cout << "Product: " << product << "\n";
}

void test_composition_and_curry() {
    std::cout << "\n=== Testing Composition and Currying ===\n";
    
    // Function composition
    auto add_one = [](int x) { return x + 1; };
    auto multiply_by_two = [](int x) { return x * 2; };
    auto composed = compose(multiply_by_two, add_one);
    
    std::cout << "compose(x2, +1)(5) = " << composed(5) << "\n"; // (5+1)*2 = 12
    
    // Currying
    auto add = [](int x, int y) { return x + y; };
    auto curried_add = curry(add);
    auto add_ten = curried_add(10);
    
    std::cout << "curry(add)(10)(5) = " << add_ten(5) << "\n"; // 10 + 5 = 15
}

void test_lambda_expressions() {
    std::cout << "\n=== Testing Lambda Expressions ===\n";
    
    std::vector<std::string> words{"hello", "functional", "programming", "world"};
    
    // Sort by length
    std::sort(words.begin(), words.end(), 
              [](const std::string& a, const std::string& b) {
                  return a.length() < b.length();
              });
    
    std::cout << "Sorted by length: ";
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << "\n";
    
    // Count words with length > 5
    int long_words = std::count_if(words.begin(), words.end(),
                                   [](const std::string& word) {
                                       return word.length() > 5;
                                   });
    std::cout << "Words with length > 5: " << long_words << "\n";
}

int main() {
    test_immutable_array();
    test_functional_utilities();
    test_composition_and_curry();
    test_lambda_expressions();
    return 0;
}