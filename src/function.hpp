#pragma once
#include "callable.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"

class Function : public Callable {
    private:
        FunctionStmt& declaration;

    public:
        Function(FunctionStmt& declaration) : declaration(declaration) {}
        
        int arity() const override {
            return declaration.params.size();
        }

        LiteralValue call(Interpreter& interpreter,const std::vector<LiteralValue>& args) override;

        std::string toString() const override {
            return "<fn" + declaration.name.lexeme + ">";
        }
};
