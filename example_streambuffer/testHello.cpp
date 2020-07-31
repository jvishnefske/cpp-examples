//
// Created by vis75817 on 7/30/20.
//
#include <gtest/gtest.h>
#include <iostream>
#include <streambuf>
//#include <locale>
#include <cstdio>

class outbuf : public std::streambuf
{
private:
    std::string m_internalBuffer;
public:
    std::string dump(){
        return m_internalBuffer;
    }
protected:
    /* central output function
     * - print characters in uppercase mode
     */
    int_type overflow (int_type c) override{
        if (c != EOF) {
            // convert lowercase to uppercase
            c = std::toupper(static_cast<char>(c),getloc());

            // and write the character to the standard output
            m_internalBuffer.push_back(c);
            if (putchar(c) == EOF) {
                return EOF;
            }
        }
        return c;
    }
};



class SerialPortFake: public std::iostream{
public:
    SerialPortFake(std::streambuf &sb): std::iostream(&sb){
    }
private:
    //implement gstream

};


class SerialPortHardware{
    //open serial port
    SerialPort(std::string portName){
        //open file handle.
        

    }

};

TEST(HelloTests, example1 ){
    std::cout<< "starting first test"<<std::endl;
    outbuf ob;
//    ob.overflow('a');
    std::ostream out(&ob);
    out << "31 hexadecimal: "
        << std::hex << 31 << std::endl;
    // note that the line ending is platform -dependent.
    const std::string expected = "31 HEXADECIMAL: 1F\\n2343 SOM INFO !!\\n";
    out << "2343 som info !!" << std::endl;
    std::cout<< "example1 complete."<<std::endl;
    EXPECT_EQ(ob.dump(), expected);
    // not much to check here. maybe add a buffer check to example class?
}

TEST(HelloTests, customClass){
    std::cout << "writing to custom base class" << std::endl;
    outbuf ob;
    SerialPortFake e(ob);
    e << " this is text to custom class1.";
//    for(int i=0; i<10; ++i){ putchar('c');}
    std::cout << "done" << std::endl;
}
TEST(HelloTests, gnuTest){
// create special output buffer
outbuf ob;
// initialize output stream with that output buffer
std::ostream out(&ob);

out << "31 hexadecimal: "
<< std::hex << 31 << std::endl;
}

TEST(DmmTest, FakeSerialPort){

}