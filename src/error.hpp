#pragma once
#include "Token.hpp"
#include<iostream>
#include <string>
bool errorFlag = false;
bool runtimeErrorFlag = false;

class ParseError : public std::exception {
    public:
        const char * what() const noexcept override {
            return "Parse Error!";
        }
};

class RuntimeError : public std::exception{
    public:
        std::string err;
        Token token;

        RuntimeError(Token token,std::string err) : token(token), err(err){}

        const char * what() const noexcept override{
            return "Runtime Error";       
    }
};

namespace Adharma{
    void report(int line,std::string where,std::string message){
        std::cout<<"[Line "<<line<<"] Error"<<where<<": "<<message;
        errorFlag = true;
    }

    void error(int line, std::string message){
        report(line,"",message);
    }

    void error(Token token, std::string message){
        if(token.type == TokenType::EOF_TOKEN) 
            report(token.line," at end",message);
        else
            report(token.line, " at '"+token.lexeme+"'",message);
    }

    void runtimeError(RuntimeError& error){
        std::cout<<"line["<<error.token.line<<"] Error at "<<error.token.lexeme<<" "<<error.err;
        runtimeErrorFlag = true;
    }
}
