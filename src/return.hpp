#include "token.hpp"
#include "util.hpp"
#include <exception>

class Return : public std::exception {
    public:
        Token keyword;
        RuntimeValue value;
        std::optional<Token> retType;

        const char* what() const noexcept override {
            return "return";
        }

        Return(Token keyword,RuntimeValue& value,std::optional<Token> retType = std::nullopt) : keyword(keyword), value(value) ,retType(retType) {}
};
