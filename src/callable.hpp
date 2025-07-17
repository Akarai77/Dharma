#pragma once

#include "expr.hpp"
#include "token.hpp"
#include "util.hpp"
#include <chrono>
#include <memory>

class Interpreter;

class Callable {
    public:
        virtual ~Callable() = default;
        virtual int arity() = 0;
        virtual std::string toString() const = 0;
        virtual RuntimeValue call(Interpreter& interpreter, const Token& name,const std::vector<Expression>& exprs) = 0;
};

class ClockFunction : public Callable {
public:
    RuntimeValue call(Interpreter& interpreter, const Token& name,const std::vector<Expression>& exprs) override {
        using namespace std::chrono;
        auto now = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
        double seconds = static_cast<double>(now) / 1000.0;
        return LiteralValue{seconds, "decimal"};
    }

    int arity() override { return 0; }

    std::string toString() const override { return "<native fn>"; }
};

class TypeOfFunction : public Callable {
    public:
        RuntimeValue call(Interpreter& interpreter, const Token& name,const std::vector<Expression>& exprs) override;

        int arity() override { return 1; }

        std::string toString() const override { return "<native fn>"; }
};
