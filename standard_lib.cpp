#pragma once
#include "functions.h"


const int Print(std::vector<Value> args) {
    if(args[0].value==0 && args[0].name!="") {
        std::cout<<args[0].name<<std::endl;
    } else {
        std::cout<<args[0].value<<std::endl;
    }
    return 0;
}

int libMain() {
    Func print = { "print", Print, 1 };
    Functions* function=Functions::get_instance();
    function->add_function(print);
    return 0;
}
