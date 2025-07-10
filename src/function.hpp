#pragma once

#include "callable.hpp"
#include "stmt.hpp"
#include "return.hpp"

class Function : public Callable {
    private:
        FunctionStmt& declaration;

    public:
        Function(FunctionStmt& declaration) : declaration(declaration) {}
        
        int arity() const override {
            return declaration.params.size();
        }

        LiteralValue call(Interpreter& interpreter, const Token& name, const std::vector<LiteralValue>& args) override;

        std::string toString() const override {
            return "<fn " + declaration.name.lexeme + " >";
        }
};
