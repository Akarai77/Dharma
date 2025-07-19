#pragma once

#include "class.hpp"
#include "error.hpp"
#include "function.hpp"
#include "util.hpp"
#include <memory>
#include <unordered_map>

class Inst : public std::enable_shared_from_this<Inst>{
    private:
        Class klass;
        std::unordered_map<std::string, LiteralValue> fields;

    public:
        Inst(Class klass) : klass(klass) {}

        std::string toString() const {
            return "<" + klass.name + " instance>";
        }

        RuntimeValue get(Token name) {
            if(fields.contains(name.lexeme)) {
                return fields[name.lexeme];
            }

            std::shared_ptr<Function> method = klass.findMethod(name.lexeme);
            if(method != nullptr) return method->bind(shared_from_this());

            throw RuntimeError(name,"Undefined property '" + name.lexeme + "'.");
        }

        void set(Token name,LiteralValue value) {
            fields[name.lexeme] = value;
        }
};

RuntimeValue Class::call(Interpreter& interpreter, const Token& name,const std::vector<Expression>& exprs) {
    Instance instance = makeShared<Inst>(*this);
    std::shared_ptr<Function> initializer = findMethod("init");
    if(initializer != nullptr) {
        initializer->bind(instance)->call(interpreter,name,exprs);
    }

    return instance;
}
