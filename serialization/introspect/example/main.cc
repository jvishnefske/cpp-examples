#include "example_generated.h"
#include <typeinfo>
#include <iosfwd>
#include <iostream>


template <class T>
class Printer{
    public:
        friend std::ostream& operator<<(std::ostream &ostr,const Printer<T> &p){
                ostr << "Printer type:" << typeid(p).name() ;
                return ostr;
        }
        friend std::ostream& operator<<(std::ostream &ostr,const T  &o){
                ostr << "Printer type:" << typeid(o).name() ;
                return ostr;
        }
        
};


// consider adding variadic template of class type or somethin idk.
template<class T>
class Generate{
        T operator()(){
            T e{};
                return e;
        }
};

using printer = Printer<MyGame::Sample::Monster>;
int main(){
    Generate<MyGame::Sample::Monster> monster;
    printer p;
    std::cout <<p <<std::endl;
    return 0;
}
