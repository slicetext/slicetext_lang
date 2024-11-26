#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <vector>
#include <cmath>

int main(){
    struct Operator{
        uint8_t precedence=0;
        uint8_t arguments=0;
    };

    std::unordered_map<char, Operator> mOps;
    mOps['^'] = {3, 2};
    mOps['/'] = {2, 2};
    mOps['%'] = {2, 2};
    mOps['*'] = {2, 2};
    mOps['+'] = {1, 2};
    mOps['-'] = {1, 2};

    std::unordered_set<std::string> mKeys;
    mKeys.insert("true");
    mKeys.insert("false");

    std::string valid_string_chars="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";

    std::string Code="#(5^2)*(12/2)\n5*5";

    struct Symbol{
        std::string token="";

        enum class Type : uint8_t {
            UNKNOWN,
            KEYWORD,
            LITERAL,
            LITERAL_NUM,
            OPERATOR,
            PAREN_OPEN,
            PAREN_CLOSE,
        }type=Type::UNKNOWN;

        Operator op;
    };

    std::deque<Symbol> sHolding;
    std::deque<Symbol> sOutput;

    Symbol previous = {"0", Symbol::Type::LITERAL_NUM,0,0};

    std::string buffer = "";
    enum class State : uint8_t {
        NONE,
        NUMBER,
        LITERAL,
        COMMENT,
    };
    State state=State::NONE;

    uint count=0;
    for(const char c : Code){
        if(buffer!=""){
            if(!std::isdigit(c) && state==State::NUMBER) {
                sOutput.push_back( { buffer, Symbol::Type::LITERAL_NUM } );
                previous=sOutput.back();
                buffer="";
                state=State::NONE;
            
            }else if(state==State::LITERAL && valid_string_chars.find(c)==valid_string_chars.npos) {
                if(mKeys.find(buffer)!=mKeys.end()) {
                    sOutput.push_back( { buffer, Symbol::Type::KEYWORD } );
                } else {
                    sOutput.push_back( { buffer, Symbol::Type::LITERAL } );
                }
                previous=sOutput.back();
                buffer="";
                state=State::NONE;
            }
        }
        if(state==State::COMMENT) {
            if(c=='\n') {
                state=State::NONE;
            }
            continue;
        }else if(std::isdigit(c) && (state==State::NONE || state==State::NUMBER)) {
            buffer.append(std::string(1,c));
            state=State::NUMBER;
        } else if(c=='(') {
            sHolding.push_front( { std::string(1,c), Symbol::Type::PAREN_OPEN} );
            previous=sHolding.front();
        } else if(c==')') {
            while(!sHolding.empty() && sHolding.front().type!=Symbol::Type::PAREN_OPEN) {
                sOutput.push_back(sHolding.front());
                sHolding.pop_front();
            }
            if(sHolding.empty()) {
                std::cout<<"Error! Mismatched Parens!"<<std::endl;
                return 0;
            }
            if(!sHolding.empty() && sHolding.front().type==Symbol::Type::PAREN_OPEN) {
                sHolding.pop_front();
            }
            Symbol previous= {std::string(1,c), Symbol::Type::PAREN_CLOSE};
        } else if (mOps.count(c)>0) {
            Operator new_op=mOps[c];

            if(c=='-' || c=='+') {
                if((previous.type!=Symbol::Type::PAREN_CLOSE && previous.type!=Symbol::Type::LITERAL_NUM) || count==0) {
                    new_op.precedence=100;
                    new_op.arguments=1;
                }
            }

            while(!sHolding.empty() && sHolding.front().type!=Symbol::Type::PAREN_OPEN) {
                if(sHolding.front().type==Symbol::Type::OPERATOR) {
                    const auto& holding_s_op = sHolding.front().op;
                    if(holding_s_op.precedence>=new_op.precedence){
                        sOutput.push_back(sHolding.front());
                        sHolding.pop_front();
                    } else {
                        break;
                    }
                }
            }
            sHolding.push_front( { std::string(1,c), Symbol::Type::OPERATOR, new_op } );
            previous=sHolding.front();
        } else if(c=='#') {
            state=State::COMMENT;
        } else {
            buffer.append(std::string(1,c));
            state=State::LITERAL;
        }
        count++;
    }
    if(buffer!="") {
        if(state==State::NUMBER){
            sOutput.push_back( { buffer, Symbol::Type::LITERAL_NUM } );
        }else if(state==State::LITERAL){
            sOutput.push_back( { buffer, Symbol::Type::LITERAL } );
        }
        previous=sOutput.back();
    }
    while(!sHolding.empty()) {
        sOutput.push_back(sHolding.front());
        sHolding.pop_front();
    }

    std::cout<<"Code: "<<Code<<std::endl;
    std::cout<<"RPN: ";
    for(const auto& s : sOutput) {
        std::cout<<s.token;
    }
    std::cout<<std::endl;

    std::deque<double> sSolve;

    for(const auto& inst : sOutput) {
        switch(inst.type) {
            case Symbol::Type::LITERAL: {
                std::cout<<"Error! Unexpected literal "<<inst.token<<std::endl;
            } break;
            case Symbol::Type::LITERAL_NUM: {
                sSolve.push_front(std::stod(inst.token));
            } break;
            case Symbol::Type::OPERATOR: {
                std::vector<double> mem(inst.op.arguments);
                for(uint8_t a=0; a<inst.op.arguments; a++) {
                    if(sSolve.empty()) {
                        std::cout<<"Error! Bad RPN!"<<std::endl;
                    } else {
                        mem[a]=sSolve[0];
                        sSolve.pop_front();
                    }
                }
                double result=0.0;
                if(inst.op.arguments==2) {
                    if(inst.token=="/") result=mem[1]/mem[0];
                    if(inst.token=="%") result=std::fmod(mem[1],mem[0]);
                    if(inst.token=="*") result=mem[1]*mem[0];
                    if(inst.token=="+") result=mem[1]+mem[0];
                    if(inst.token=="-") result=mem[1]-mem[0];
                    if(inst.token=="^") result=std::pow(mem[1],mem[0]);
                } else if(inst.op.arguments==1) {
                    if(inst.token=="+") result=+mem[0];
                    if(inst.token=="-") result=-mem[0];
                }
                sSolve.push_front(result);
            } break;
            case Symbol::Type::KEYWORD: {
                if(inst.token=="true" || inst.token=="false") break;
            } break;
        }
    }

    std::cout<<"Result: "<<std::to_string(sSolve[0])<<std::endl;
}
