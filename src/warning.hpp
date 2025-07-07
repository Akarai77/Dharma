#pragma once

#include "SourceManager.hpp"
#include "Token.hpp"
#include <string>

class Warning {
    public:
        virtual const std::string what() const {
            return "Warning";
        }

        virtual std::string message() const = 0;
        virtual ~Warning() = default;
};

class SemiColonWarning : public Warning {
    private:
        Token token;
        std::string contextLine;

    public:
        SemiColonWarning(const Token& token) 
            : token(token) {
                contextLine = SourceManager::instance().getLine(token.line);
            }

        const std::string what() const override {
            return "SemiColonWarning";
        }

        std::string message() const override {
            std::string caretLine(token.column, ' ');
            caretLine += "^";

            return WHITE "[line " + std::to_string(token.line) +
                ", column " + std::to_string(token.column) +
                "] " YELLOW + what() + RESET ": Implicit semicolon inserted.\n\n\t\t" +
                contextLine + "\n\t\t" YELLOW + caretLine + RESET + "\n";
        }
};

class ImplicitConversionWarning : public Warning {
    private:
        Token token;
        std::string targetType;
        std::string actualType;
        std::string contextLine;

    public:
        ImplicitConversionWarning(const Token& token,const std::string& t,const std::string& a)
            : token(token), targetType(t), actualType(a) {
                contextLine = SourceManager::instance().getLine(token.line);
            }

        const std::string what() const override {
            return "ImplicitConversionWarning";
        }

        std::string message() const override {
            std::string caretLine(token.column, ' ');
            caretLine += "^";

            return WHITE "[line " + std::to_string(token.line) +
                ", column " + std::to_string(token.column) +
                "] " YELLOW + what() + RESET ": Implicit conversion from '" +
                actualType + "' to '" + targetType + "'.\n\n\t\t" +
                contextLine + "\n\t\t" YELLOW + caretLine + RESET + "\n";
        }
};
