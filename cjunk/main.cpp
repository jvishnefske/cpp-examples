#include <iostream>
#include <gtest/gtest.h>

template<class Impl>
class I2cInterface: Impl{
    void read(uint8_t addr){

    }
};

template<class T>
void log(T c){std::cout << c;}

class I2cLibImpl{
    I2cLibImpl() {
        log("lib constructor");
    }

    ~I2cLibImpl(){
        log("lib destructor");
    }

};

//explicit instanciation.
class

TEST(tests, i2cTest){
    std::cout << "i2cTest" << std::endl;
    I2cInterface
}
TEST(tests, test1){
    ASSERT_TRUE(true);
}
