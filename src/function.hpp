#pragma once

#include "callable.hpp"
#include "environment.hpp"
#include "stmt.hpp"
#include "return.hpp"
#include "util.hpp"

class Function : public Callable {
    private:
        FunctionStmt& declaration;
        Environment* closure;
        bool isInitializer;

    public:
        Function(FunctionStmt& declaration,Environment*& closure,bool isInitializer) :
            declaration(declaration), closure(closure), isInitializer(isInitializer) {}
        
        int arity() override {
            return declaration.params.size();
        }

        RuntimeValue call(Interpreter& interpreter, const Token& name,const std::vector<Expression>& exprs) override;

        std::shared_ptr<Function> bind(Instance instance) {
            Environment* environment = new Environment(closure);
            environment->define("this",instance,"class");
            return makeShared<Function>(declaration,environment,isInitializer);
        }

        std::string toString() const override {
            return "<fn " + declaration.name.lexeme + " >";
        }
};
