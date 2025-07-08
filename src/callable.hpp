#pragma once

#include "util.hpp"
#include <chrono>
#include <memory>

class Interpreter;
using CallAble = std::shared_ptr<Callable>; 

template <typename T, typename... Args>
std::shared_ptr<T> makeCallable(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

class Callable {
    public:
        virtual ~Callable() = default;
        virtual int arity() const = 0;
        virtual std::string toString() const = 0;
        virtual LiteralValue call(Interpreter& interpreter,const std::vector<LiteralValue>& args) = 0;
};

class ClockFunction : public Callable {
public:
    LiteralValue call(Interpreter& interpreter, const std::vector<LiteralValue>& args) override {
        using namespace std::chrono;
        auto now = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
        double seconds = static_cast<double>(now) / 1000.0;
        return LiteralValue{seconds, "decimal"};
    }

    int arity() const override { return 0; }

    std::string toString() const override { return "<native fn>"; }
};

