/**
 * @startuml sequence.png
 * actor  Bob #red
 *
 * Bob->Alice: syn
 * Alice->Bob : synack
 * Bob -> Alice: ack
 * @enduml

 *
 * @startuml
 * left to right direction
 * Vst as v
 * package Loop{
 *  usecase configItems
 *  usecase genlist
 * }
 * v-->configItems
 * v-->genlist
 * GenIterm1 -> GenItem2
 * @enduml
 *
 * @startuml
 * saturday are closed
 * sunday are closed
 * project starts on 2021-4-1
 * [develop demo story] as [D] lasts 2 days
 * [break down tasks] as [T] lasts 2 days
 * [T] starts after [D]'s end
 * [prototype design] lasts 10 days
 * [buffer] lasts 10 days
 * [buffer] starts on 2021-5-1
 * @enduml
 */

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
