#pragma once
#include "Token.hpp"
#include <exception>
#include <string>

class Adharma : public std::exception {
    public:
        virtual const char* what() const noexcept override {
            return "AdharmaError";
        }

        virtual std::string message() const = 0;
};

class LexicalError : public Adharma {
    int line;
    std::string msg;
    public:
        LexicalError(const int& line,const std::string& msg)
            : line(line), msg(msg) {}

        const char* what() const noexcept override {
            return "LexicalError";
        }

        std::string message() const override {
            return "[line " + std::to_string(line) + "] LexicalError : "+ msg;
        }
};

class ParseError : public Adharma {
    Token token;
    std::string msg;
    public:
        ParseError(const Token& token, const std::string& msg)
            : token(token), msg(msg) {}

        const char* what() const noexcept override {
            return "ParseError";
        }

        std::string message() const override {
            return "[line " + std::to_string(token.line) + "] ParseError at '" + token.lexeme + "': " + msg;
        }
};

class RuntimeError : public Adharma {
    Token token;
    std::string msg;
    public:
        RuntimeError(const Token& token, const std::string& msg)
            : token(token), msg(msg) {}

        const char* what() const noexcept override {
            return "RuntimeError";
        }

        std::string message() const override {
            return "[line " + std::to_string(token.line) + "] RuntimeError at '" + token.lexeme + "': " + msg;
        }
};

