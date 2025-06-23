#pragma once
#include <any>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include"tokenType.hpp"
using LiteralType = std::optional<std::variant<int, double, std::string, bool>>;
using LiteralValue = std::pair<std::any,std::string>;

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

LiteralValue getLiteralData(const LiteralType& expr) {
    LiteralValue result;

    std::visit([&result](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        result.first = val;

        if constexpr (std::is_same_v<T, int>) {
            result.second = "int";
        } else if constexpr (std::is_same_v<T, double>) {
            result.second = "decimal";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result.second = "string";
        } else if constexpr (std::is_same_v<T, bool>) {
            result.second = "boolean";
        } else {
            result.second = "nil";
        }
    }, expr.value());

    return result;
}

