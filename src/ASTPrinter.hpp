#pragma once

#include "expr.hpp"
#include <string>
#include <any>
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
                ((out << " " << std::any_cast<std::string>(print(exprs).first)), ...);
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
            if(expr.literal.second == "int")
                return {std::to_string(std::any_cast<int>(expr.literal.first)), "string"};
            if(expr.literal.second == "float")
                return {std::to_string(std::any_cast<float>(expr.literal.first)), "string"};
            if(expr.literal.second == "double")
                return {std::to_string(std::any_cast<double>(expr.literal.first)), "string"};
            if(expr.literal.second == "boolean")
                return {std::any_cast<bool>(expr.literal.first) ? "true" : "false", "string"};

            return {"nil", "string"};
        }

};

