#pragma once

#include "expr.hpp"
#include <string>
#include <sstream>

class ASTPrinter : public ExprVisitor {
    public:
        LiteralValue print(Expr& expr) {
            return expr.accept(*this);
        }

    private:
        template<typename... Exprs>
            LiteralValue format(const std::string& name,Exprs&... exprs) {
                std::ostringstream out;
                out << "(" << name;
                ((out << " " << std::get<std::string>(print(exprs).first)), ...);
                out << ")";
                LiteralValue value;
                return {out.str(),"string"};
            }

        LiteralValue visitBinaryExpr(BinaryExpr& expr) override {
            return format(expr.Operator.lexeme, *expr.left, *expr.right);
        }

        LiteralValue visitUnaryExpr(UnaryExpr& expr) override {
            return format(expr.Operator.lexeme, *expr.right);
        }

        LiteralValue visitGroupingExpr(GroupingExpr& expr) override {
            return format("group", *expr.expression);
        }

        LiteralValue visitAssignExpr(AssignExpr& expr) override {
            return format(expr.name.lexeme, *expr.value);
        }

        LiteralValue visitVariableExpr(VariableExpr& expr) override{
            return format(expr.name.lexeme);
        }
 
        LiteralValue visitLiteralExpr(LiteralExpr& expr) override {
            if(expr.literal.second == "string")
                return expr.literal;
            if(expr.literal.second == "integer")
                return {std::get<Integer>(expr.literal.first).toString(), "string"};
            if(expr.literal.second == "decimal")
                return {std::to_string(std::get<double>(expr.literal.first)), "string"};
            if(expr.literal.second == "BigDecimal")
                return {std::get<BigDecimal>(expr.literal.first).toString(), "string"};
            if(expr.literal.second == "boolean")
                return {std::get<bool>(expr.literal.first) ? std::string("true") : std::string("false"), "string"};

            return {Nil(), "string"};
        }

};

