#pragma once
#include <cstdio>
#include <iostream>
#include <vector>
struct Value{
    double value=0;
    std::string name="";
};
struct Func{
    std::string name;
    const int(*callback)(std::vector<Value> args);
    int amt_args;
};
class Functions{
    public:
        static Functions* get_instance() {
            static Functions* instance;
            return instance;
        }
        static void add_function(Func function) {
            functions.emplace_back(function);
        }
        static std::vector<Func>* get_functions() {
            return &functions;
        }
        Functions(const Functions&) = delete;
        Functions& operator=(const Functions&) = delete;
    private:
        inline static std::vector<Func> functions;
        Functions(){}
        ~Functions(){}
};
