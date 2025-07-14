#pragma once

#include "expr.hpp"
#include <string>
#include <sstream>

class ASTPrinter : public std::enable_shared_from_this<ASTPrinter>, ExprVisitor {
    public:
        RuntimeValue print(std::shared_ptr<Expr> expr) {
            return expr->accept(*this);
        }

    private:
        template<typename... Exprs>
            RuntimeValue format(const std::string& name, std::shared_ptr<Exprs>... exprs) {
                std::ostringstream out;
                out << "(" << name;
                (..., (out << " " << std::get<std::string>(print(exprs).first)));
                out << ")";
                return LiteralValue{out.str(), "string"};
            }

        RuntimeValue visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) override {
            return format(expr->Operator.lexeme, expr->left, expr->right);
        }

        RuntimeValue visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) override {
            return format(expr->Operator.lexeme, expr->right);
        }

        RuntimeValue visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) override {
            return format("group", expr->expression);
        }

        RuntimeValue visitAssignExpr(std::shared_ptr<AssignExpr> expr) override {
            return format(expr->name.lexeme, expr->value);
        }

        RuntimeValue visitVariableExpr(std::shared_ptr<VariableExpr> expr) override{
            return format(expr->name.lexeme);
        }

        RuntimeValue visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) override {
            return format(expr->Operator.lexeme, expr->left, expr->right);
        }

        RuntimeValue visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) override {
            if(expr->literal.second == "string")
                return expr->literal;
            if(expr->literal.second == "integer")
                return LiteralValue{std::get<Integer>(expr->literal.first).toString(), "string"};
            if(expr->literal.second == "decimal")
                return LiteralValue{std::to_string(std::get<double>(expr->literal.first)), "string"};
            if(expr->literal.second == "BigDecimal")
                return LiteralValue{std::get<BigDecimal>(expr->literal.first).toString(), "string"};
            if(expr->literal.second == "boolean")
                return LiteralValue{std::get<bool>(expr->literal.first) ? std::string("true") : std::string("false"), "string"};

            return LiteralValue{std::string("nil"), "string"};
        }

};

