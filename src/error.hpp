#pragma once
#include "sourceManager.hpp"
#include "token.hpp"
#include <exception>
#include <string>

class Adharma : public std::exception {
    public:
        virtual const char* what() const noexcept override {
            return "AdharmaError";
        }

        virtual std::string message() const = 0;
};

class SyntaxError : public Adharma {
    private:
        int line;
        int column;
        std::string msg;
        std::string contextLine;

    public:
        SyntaxError(int line,int column,const std::string& msg)
            : line(line), column(column), msg(msg) {
                contextLine = SourceManager::instance().getLine(line);
            }

        const char* what() const noexcept override {
            return "SyntaxError";
        }

        std::string message() const override {
            std::string caretLine(column, ' ');
            caretLine += "^";
            return WHITE "[line " + std::to_string(line) + ", column " +
                std::to_string(column) + "] " RED + what() + RESET ": " + msg +
                "\n\n\t\t" + contextLine + "\n\t\t" RED + caretLine + RESET + "\n";
        }
};

class ParseError : public Adharma {
    private:
        Token token;
        std::string msg;
        std::string contextLine;
    
    public:
        ParseError(const Token& token, const std::string& msg)
            : token(token), msg(msg) {
                contextLine = SourceManager::instance().getLine(token.line);
            }

        const char* what() const noexcept override {
            return "ParseError";
        }

        std::string message() const override {
            std::string caretLine(token.column, ' ');
            caretLine += "^";
            return WHITE "[line " + std::to_string(token.line) +
                   ", column " + std::to_string(token.column) +
                   "] " RED + what() + RESET ": '" + token.lexeme + "': " +
                   msg + "\n\n\t\t" + contextLine + "\n\t\t" RED + caretLine + RESET + "\n";
        }
};

class RuntimeError : public Adharma {
    private:
        Token token;
        std::string msg;
        std::string contextLine;
    
    public:
        RuntimeError(const Token& token, const std::string& msg)
            : token(token), msg(msg) {
                contextLine = SourceManager::instance().getLine(token.line);
            }

        const char* what() const noexcept override {
            return "RuntimeError";
        }

        std::string message() const override {
            std::string caretLine(token.column, ' ');
            for(int i=0;i<token.lexeme.size();i++)
                caretLine += "^";

            return WHITE "[line " + std::to_string(token.line) +
                    ", column " + std::to_string(token.column) +
                    "] " RED + what() + RESET ": '" + token.lexeme + "': " +
                    msg + "\n\n\t\t" + contextLine + "\n\t\t" RED + caretLine + RESET + "\n";
        }
};

