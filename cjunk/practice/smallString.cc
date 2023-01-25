#include <numeric>
#include <array>
#include <cmath>
#include <vector>
#include <complex>
#include <algorithm>
#include <map>
#include <string>
#include <variant>
#include <iostream>

struct SmallString{
    constexpr SmallString(std::string_view str=""){
        // we depend on all null initializion to ensure proper termination where needed.
        characters = {};
        auto n = std::min(str.size(), characters.size());
        // requires c++20 for this to be constexpr
        std::copy_n(std::begin(str), n,std::begin(characters));
    }
    constexpr size_t size() const{

        auto resultIt = std::find(std::begin(characters), std::end(characters), '\0');
        size_t i = resultIt - std::begin(characters);

        return i;
    }

    constexpr std::string_view operator*() const{
        return std::string_view(characters.begin(), size());
    }
private:
    std::array<char, 8> characters;
};


struct value{
    using SmallString = std::array<char,8>;
    using SmallArray = std::array<value,8>;
    std::variant<std::monostate, SmallString> v;
};


int main(){
    constexpr value l{};
    constexpr SmallString s{};
    static_assert(s.size()==0, " empty string should have zero size");
    static_assert(*s == "", "empty string should be retrievable at compile time");
    constexpr SmallString a{"A"};
    static_assert(*a == "A", "simple string check");
    std::cout << *a << "size: "<< a.size() <<std::endl;
    static_assert(sizeof(l) >0,"too small");
    static_assert(sizeof(l)<=16, "too big");
    (void) l;
    return a.size();
}