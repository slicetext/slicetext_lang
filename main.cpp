#include <array>
#include <cctype>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>
#include <deque>
#include <cmath>
struct Operator{
    uint8_t precedence;
    uint8_t arguments;
};
struct Token{
    enum class Type {
        UNKNOWN,
        NUM_LITERAL,
        STRING_LITERAL,
        OPERATOR,
        SYMBOL,
        PAREN_OPEN,
        PAREN_CLOSE,
        SEPERATOR,
    };

    Type type=Type::UNKNOWN;
    std::string symbol;
    double value=0.0;
    Operator op;

    void prettyStr() const {
        switch (type) {
            case Token::Type::UNKNOWN:
                std::cout<<"[Unknown "<<symbol<<"]"<<std::endl;
                break;
            case Token::Type::NUM_LITERAL:
                std::cout<<"[Literal Number "<<symbol<<" ("<<value<<") ]"<<std::endl;
                break;
            case Token::Type::STRING_LITERAL:
                std::cout<<"[Literal String "<<symbol<<" ]"<<std::endl;
                break;
            case Token::Type::OPERATOR:
                std::cout<<"[Operator "<<symbol<<"]"<<std::endl;
                break;
            case Token::Type::SYMBOL:
                std::cout<<"[Symbol "<<symbol<<"]"<<std::endl;
                break;
            case Token::Type::PAREN_OPEN:
                std::cout<<"[Open Paren]"<<std::endl;
                break;
            case Token::Type::PAREN_CLOSE:
                std::cout<<"[Close Paren]"<<std::endl;
                break;
            case Token::Type::SEPERATOR:
                std::cout<<"[Seperator]"<<std::endl;
        }
    }
};
class CompileError : public std::exception {
    public:
        CompileError(std::string msg) {
            message=msg;
        }
        const char* what() {
            return message.c_str();
        }
    private:
        std::string message="";
};
class Compiler {
    protected:
        std::unordered_map<std::string, Operator> mOps;
    public:
        Compiler() {
            mOps["^"] = {5, 2};
            mOps["*"] = {5, 2};
            mOps["/"] = {5, 2};
            mOps["%"] = {5, 2};
            mOps["+"] = {4, 2};
            mOps["-"] = {4, 2};
        }
        std::vector<Token> Lex(std::string code) {
            std::vector<Token> tokens;
            if(code.empty()) {
                throw CompileError("Lexer: No Input Provided");
                return tokens;
            }
            enum class State {
                NEWTOKEN,
                SYMBOL,
                NUM_LITERAL,
                STRING_LITERAL,
                F_NUM_LITERAL,
                HEX_NUM_LITERAL,
                BIN_NUM_LITERAL,
                OPERATOR,
                ENDTOKEN,
                OPEN_PAREN,
                CLOSE_PAREN,
                SEPERATOR,
            };
            State curr_state=State::NEWTOKEN;
            State next_state=State::NEWTOKEN;
            uint16_t cur=0;
            std::string curr_token="";
            std::string f_num_tok="";
            Token tok_curr;
            int16_t paren_balence=0;
            bool num_lit_point_used=false;
            uint16_t len=code.length();
            while(cur<len) {
                char sCur=code[cur];
                std::string operators  =  "*/+-%^=";
                std::string whitespace =  "\t\r\v\f ";
                std::string symbols    =  "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
                std::string hexLit     =  "0123456789abcdefABCDEF";
                std::string binLit     =  "01";
                bool ispar=sCur==')';
                switch (curr_state) {
                    case (State::NEWTOKEN):
                    {
                        //Reset
                        curr_token="";
                        f_num_tok="";
                        tok_curr = {Token::Type::UNKNOWN, ""};
                        num_lit_point_used=false;

                        //First Char Analysis
                        //Whitespace
                        if(whitespace.find(sCur)!=whitespace.npos) {
                            ++cur;
                            next_state=State::NEWTOKEN;
                        }
                        //Numeric Literals
                        else if(std::isdigit(sCur)) {
                            if(sCur=='0') {
                                curr_token+=sCur;
                                next_state=State::F_NUM_LITERAL;
                                ++cur;
                            } else {
                                //curr_token = sCur;
                                next_state=State::NUM_LITERAL;
                                //++cur;
                            }
                        }
                        //Operators
                        else if(operators.find(sCur)!=operators.npos) {
                            next_state=State::OPERATOR;
                        }
                        else if(sCur=='(') {
                            next_state=State::OPEN_PAREN;
                        }
                        else if(sCur==')') {
                            next_state=State::CLOSE_PAREN;
                        }
                        //Seperator
                        else if(sCur==',') {
                            next_state=State::SEPERATOR;
                        }
                        //String
                        else if(sCur=='\"') {
                            next_state=State::STRING_LITERAL;
                            ++cur;
                        }
                        //Symbol
                        else if(symbols.find(sCur)!=symbols.npos) {
                            next_state=State::SYMBOL;
                        }
                    } break;
                    case (State::F_NUM_LITERAL):
                    {
                        curr_token+=sCur;
                        if(sCur=='x') {
                            next_state=State::HEX_NUM_LITERAL;
                        } else if(sCur=='b') {
                            next_state=State::BIN_NUM_LITERAL;
                        } else if(isdigit(sCur) || sCur=='.') {
                            next_state=State::NUM_LITERAL;
                        } else {
                            //throw CompileError("Lexer: Bad Numeric Literal Construction");
                            tok_curr = {Token::Type::NUM_LITERAL, curr_token, 0};
                            //BAD PRACTICE!!!! Idk how to fix
                            --cur;
                            next_state=State::ENDTOKEN;
                        }
                        ++cur;
                    } break;
                    case (State::HEX_NUM_LITERAL):
                    {
                        if(hexLit.find(sCur)!=hexLit.npos) {
                            curr_token+=sCur;
                            f_num_tok+=sCur;
                            ++cur;
                            next_state=State::HEX_NUM_LITERAL;
                        } else {
                            next_state=State::ENDTOKEN;
                            tok_curr = {Token::Type::NUM_LITERAL, curr_token};
                            tok_curr.value = double(std::stoll(f_num_tok, nullptr, 16));
                        }
                    } break;
                    case (State::BIN_NUM_LITERAL):
                    {
                        if(binLit.find(sCur)!=binLit.npos) {
                            curr_token+=sCur;
                            f_num_tok+=sCur;
                            ++cur;
                            next_state=State::BIN_NUM_LITERAL;
                        } else {
                            next_state=State::ENDTOKEN;
                            tok_curr = {Token::Type::NUM_LITERAL, curr_token};
                            tok_curr.value = double(std::stoll(f_num_tok, nullptr, 2));
                        }
                    } break;
                    case (State::NUM_LITERAL):
                    {
                        if(std::isdigit(sCur) || sCur=='.') {
                            if(num_lit_point_used && sCur=='.') throw CompileError("Lexer: Too many decimals in numeric literal");
                            if(sCur=='.') num_lit_point_used=true;
                            curr_token += sCur;
                            ++cur;
                            next_state=State::NUM_LITERAL;
                        } else {
                            next_state=State::ENDTOKEN;
                            tok_curr = {Token::Type::NUM_LITERAL, curr_token};
                            //Convert value to double
                            tok_curr.value=std::stod(curr_token);
                        }
                    } break;
                    case(State::STRING_LITERAL):
                    {
                        if(sCur!='\"') {
                            curr_token+=sCur;
                            ++cur;
                            next_state=State::STRING_LITERAL;
                        } else {
                            ++cur;
                            tok_curr = {Token::Type::STRING_LITERAL, curr_token};
                            next_state=State::ENDTOKEN;
                        }
                    } break;
                    case (State::OPERATOR):
                    {
                        if(operators.find(sCur)!=operators.npos) {
                            if(mOps.count(curr_token+sCur)>0) {
                                curr_token+=sCur;
                                ++cur;
                            } else {
                                if(mOps.count(curr_token)>0) {
                                    tok_curr = {Token::Type::OPERATOR, curr_token};
                                    next_state=State::ENDTOKEN;
                                } else {
                                    curr_token+=sCur;
                                    ++cur;
                                }
                            }
                        } else {
                            if(mOps.count(curr_token)>0) {
                                tok_curr = {Token::Type::OPERATOR, curr_token, 0, mOps[curr_token]};
                                next_state=State::ENDTOKEN;
                            } else {
                                throw CompileError("Lexer: Invalid Operator: "+curr_token);
                            }
                        }
                    } break;
                    case (State::OPEN_PAREN):
                    {
                        curr_token+=sCur;
                        ++cur;
                        paren_balence++;
                        tok_curr = {Token::Type::PAREN_OPEN, curr_token};
                        next_state=State::ENDTOKEN;
                    } break;
                    case (State::CLOSE_PAREN): {
                        if(curr_state==State::CLOSE_PAREN){
                            curr_token+=sCur;
                            ++cur;
                            paren_balence--;
                            tok_curr = {Token::Type::PAREN_CLOSE, curr_token};
                            next_state=State::ENDTOKEN;
                        }
                    } break;
                    case (State::SEPERATOR):
                    {
                        curr_token+=sCur;
                        ++cur;
                        tok_curr = {Token::Type::SEPERATOR, curr_token};
                        next_state=State::ENDTOKEN;
                    } break;
                    case (State::SYMBOL): {
                        if(symbols.find(sCur)!=symbols.npos) {
                            curr_token+=sCur;
                            ++cur;
                        } else {
                            tok_curr = {Token::Type::SYMBOL, curr_token};
                            next_state=State::ENDTOKEN;
                        }
                    } break;
                    case (State::ENDTOKEN):
                    {
                        tokens.push_back(tok_curr);
                        next_state=State::NEWTOKEN;
                        //++cur;
                    } break;
                }
                curr_state=next_state;
            }
            if(paren_balence!=0) {
                throw CompileError("Lexer: Unbalenced Parenthesis!");
            }
            if(curr_state==State::STRING_LITERAL) {
                throw CompileError("Lexer: String not Closed");
            }
            return tokens;
        }
        std::deque<Token> Parse(std::vector<Token> code) {
            std::deque<Token> stkHolding;
            std::deque<Token> stkOutput;
            Token previous = {Token::Type::NUM_LITERAL, "0", 0};
            uint count=0;
            for(const Token c : code) {
                switch(c.type) {
                    case(Token::Type::NUM_LITERAL):
                    {
                        stkOutput.push_back(c);
                        previous=stkOutput.back();
                    } break;
                    case(Token::Type::PAREN_OPEN):
                    {
                        stkHolding.push_front(c);
                        previous=stkHolding.front();
                    } break;
                    case(Token::Type::PAREN_CLOSE):
                    {
                        while(!stkHolding.empty() && stkHolding.front().type!=Token::Type::PAREN_OPEN) {
                            stkOutput.push_back(stkHolding.front());
                            stkHolding.pop_front();
                        }
                        if(!stkHolding.empty() && stkHolding.front().type==Token::Type::PAREN_OPEN) {
                            stkHolding.pop_front();
                        }
                    } break;
                    case(Token::Type::OPERATOR):
                    {
                        Operator tNewOp=mOps[c.symbol];
                        if(c.symbol=="-" or c.symbol=="+") {
                            if((previous.type==Token::Type::PAREN_CLOSE || previous.type==Token::Type::NUM_LITERAL) || count==0) {
                                tNewOp.precedence=100;
                                tNewOp.arguments=1;
                            }
                        }
                        // Flush holding stack if lower precedence
                        while(!stkHolding.empty() && stkHolding.front().type!=Token::Type::PAREN_OPEN) {
                            if(stkHolding.front().type==Token::Type::OPERATOR) {
                                if(stkHolding.front().op.precedence>=tNewOp.precedence){
                                    stkOutput.push_back(stkHolding.front());
                                    stkHolding.pop_front();
                                } else {
                                    break;
                                }
                            }
                        }
                        stkHolding.push_front(c);
                        previous=stkHolding.front();
                    }
                }
                count++;
            }
            while(!stkHolding.empty()) {
                stkOutput.push_back(stkHolding.front());
                stkHolding.pop_front();
            }
            return stkOutput;
        }
        double Solve(std::deque<Token> tokens) {
            std::deque<double> stkSolve;
            for(const auto& t : tokens) {
                switch(t.type) {
                    case(Token::Type::NUM_LITERAL):
                    {
                        stkSolve.push_front(t.value);
                    } break;
                    case(Token::Type::OPERATOR):
                    {
                        std::vector<double> mem(t.op.arguments);
                        for(u_int8_t a=0; a<t.op.arguments; a++) {
                            if(stkSolve.empty()) {
                                throw CompileError("Solver: Bad RPN!");
                            } else {
                                mem[a]=stkSolve[0];
                                stkSolve.pop_front();
                            }
                        }
                        double result=0.0;
                        if(t.op.arguments==2) {
                            if(t.symbol=="/") result=mem[1]/mem[0];
                            if(t.symbol=="%") result=std::fmod(mem[1],mem[0]);
                            if(t.symbol=="*") result=mem[1]*mem[0];
                            if(t.symbol=="+") result=mem[1]+mem[0];
                            if(t.symbol=="-") result=mem[1]-mem[0];
                            if(t.symbol=="^") result=std::pow(mem[1],mem[0]);
                        } else if(t.op.arguments==1) {
                            if(t.symbol=="+") result=+mem[0];
                            if(t.symbol=="-") result=-mem[0];
                        }
                        stkSolve.push_front(result);
                    } break;
                }
            }
            return stkSolve[0];
        }
};

const std::string RESET_COLOR    = "\033[0m";
const std::string ERROR_COLOR    = "\033[1;31m";
const std::string FAILURE_COLOR  = "\033[0;31m";
const std::string SUCCESS_COLOR  = "\033[0;32m";

int main()
{
    while(true) {
        std::string code;
        std::cout<<">> "<<RESET_COLOR;
        std::getline(std::cin,code);
        code+=" ";
        Compiler compiler;
        try {
            auto tokens=compiler.Lex(code);
            for(const auto& token : tokens) {
                std::cout<<RESET_COLOR;
                token.prettyStr();
            }
            auto rpn=compiler.Parse(tokens);
            for(const auto& token : rpn) {
                if(token.type==Token::Type::NUM_LITERAL) {
                    std::cout<<RESET_COLOR<<token.value<<" ";
                } else {
                    std::cout<<RESET_COLOR<<token.symbol<<" ";
                }
            }
            std::cout<<std::endl;
            double result = compiler.Solve(rpn);
            std::cout<<result<<std::endl;
            std::cout<<SUCCESS_COLOR;
        } catch (CompileError& e) {
            std::cout<<ERROR_COLOR<<e.what()<<FAILURE_COLOR<<std::endl;
        }
    }
    return 0;
}
