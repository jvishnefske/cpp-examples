#include <iostream>
#include <unistd.h>
int main(){
    if(isatty(0)){
        std::cout << "stdin is a terminal" << std::endl;
    }
    else{
        std::cout << "stdin is not a terminal" << std::endl;
    }
    if(isatty(1)){
        std::cout << "stdout is a terminal" << std::endl;
    }
    else{
        std::cout << "stdout is not a terminal" << std::endl;
    }
    if(isatty(2)){
        std::cout << "stderr is a terminal" << std::endl;
    }
    else{
        std::cout << "stderr is not a terminal" << std::endl;
    }
    return 0;
}
