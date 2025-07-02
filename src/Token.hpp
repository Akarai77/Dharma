#pragma once
#include <iostream>
#include <string>
#include <variant>
#include "tokenType.hpp"
#include "util.hpp"

class Token{
    public:
        TokenType type;
        std::string lexeme;
        LiteralType literal;
        int line;
        int column;

        Token(TokenType type,std::string lexeme,LiteralType literal,int line,int column = -1) :
            type(type), lexeme(lexeme), literal(literal), line(line), column(column) {}

        friend std::ostream& operator<<(std::ostream& out,const Token& token);
};

std::ostream& operator<<(std::ostream& out, const Token& token) {
    out << "< " << static_cast<int>(token.type) << " " << token.lexeme << " ";
    if (token.literal.has_value()) {
        std::visit([&out](const auto& literal) {
            out << literal << " ";
        }, *token.literal);
    }
    out<<"> ";
    return out;
}

