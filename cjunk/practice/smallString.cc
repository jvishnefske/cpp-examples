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
        characters = {};
        for(auto i=0u; i<characters.size() && i<str.size();++i){
            characters[i] = str[i];
        }
    }
    constexpr size_t size() const{
        size_t i;
        for(i = 0; i<characters.size() && characters[i]!='\0'; ++i){

        }
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
    (void) s;
    (void) l;
    return a.size();
}
