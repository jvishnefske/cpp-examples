//
// Created by j on 10/23/21.
//

#ifndef MIDDLEWARE_FIRST_H
#define MIDDLEWARE_FIRST_H

#include <iostream> // Added missing include for std::cout

class first {
    public:
        void otherFunction(bool value) const { // Added 'const' as the function does not modify the object's state.
            if(value){
                std::cout << "true" << std::endl;
            }else{
                std::cout << "false" <<std::endl;
            }
        }
};


#endif //MIDDLEWARE_FIRST_H
