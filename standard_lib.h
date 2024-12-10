#pragma once
#include "functions.h"
#include <string>


const Value Print(std::vector<Value> args) {
    if(args[0].value==0 && args[0].str_value!="") {
        std::cout<<args[0].str_value;
    } else if(args[0].value!=0) {
        std::cout<<args[0].value;
    } else {
        std::cout<<args[0].value;
    }
    return {0, "", true};
}
const Value Println(std::vector<Value> args) {
    Print(args);
    std::cout<<std::endl;
    return {0, "", true};
}

const Value Str(std::vector<Value> args) {
    return { 0, "", false, std::to_string(args[0].value) };
}

int libMain() {
    Functions* function=Functions::get_instance();
    // Printing
    Func println = { "println", Println, 1 };
    function->add_function(println);
    Func print = { "print", Print, 1 };
    function->add_function(print);
    // Type converions
    Func str = {"str", Str, 1 };
    function->add_function(str);
    return 0;
}
