#include <iostream>
#include <gtest/gtest.h>

template<class Impl>
class I2cInterface: Impl{
public:
    void read(uint8_t addr){

    }
};

template<class T>
void log(T c){std::cout << c;}

class I2cLibImpl{
public:
    I2cLibImpl() {
        log("lib constructor");
    }

    ~I2cLibImpl(){
        log("lib destructor");
    }

};

//explicit instanciation.
typedef class I2cInterface<I2cLibImpl> I2cHardware;

TEST(tests, i2cTest){
    std::cout << "i2cTest" << std::endl;
    I2cHardware h;
    h.read(0);
}
TEST(tests, test1){
    ASSERT_TRUE(true);
}
