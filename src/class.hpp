#pragma once

#include "callable.hpp"
#include "function.hpp"
#include "util.hpp"
#include <unordered_map>

class Class : public Callable {
    public:
        std::string name;
        std::unordered_map<std::string,std::shared_ptr<Function>> methods;

        Class(std::string name,std::unordered_map<std::string,std::shared_ptr<Function>>& methods) : name(name), methods(methods) {}

        std::string toString() const override {
            return "<class " + name + ">";
        }

        std::shared_ptr<Function> findMethod(std::string name) {
            if(methods.contains(name)) {
                return methods[name];
            }

            return nullptr;
        }

        RuntimeValue call(Interpreter& interpreter, const Token& name,const std::vector<Expression>& exprs) override;

        int arity() override{
            std::shared_ptr<Function> initializer = findMethod("init");
            if(initializer == nullptr) return 0;
            return initializer->arity();
        }
};
