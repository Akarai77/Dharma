#pragma once

#include "Token.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "tokenType.hpp"
#include "environment.hpp"
#include <any>
#include <cmath>
#include <minwindef.h>
#include <string>
#include <vector>
#include "error.hpp"

class Interpreter : public ExprVisitor, public StmtVisitor{
    private:
        Environment* environment = new Environment();

        LiteralValue evaluate(Expression& expression){
            return expression->accept(*this);
        }

        void execute(Statement& statement){
            statement->accept(*this);
        }

        void executeBlock(std::vector<Statement>& statements,Environment* newEnvironment){
            EnvSwitch Switch(this->environment,newEnvironment);

            for(auto& statement : statements){
                execute(statement);
            }
        }

        LiteralValue isTruthy(LiteralValue literal) const {
            if(literal.second == "nil") return {false,"bool"};
            if(literal.second == "bool") return {!std::any_cast<bool>(literal.first),"bool"};
            return {true,"boolean"};
        }

        LiteralValue isEqual(const std::any& l,const std::any& r) const {
            if(!l.has_value() && !r.has_value()) return {true,"bool"};
            if(!l.has_value()) return {false,"bool"};

            return {true,"boolean"};
        }

        int getPriority(std::string type){
            if(type == "boolean") return 0;
            if(type == "int") return 1;
            if(type == "decimal") return 2;
            if(type == "string") return 3;
            return -1;
        }

        LiteralValue promoteType(const LiteralValue& operand,std::string targetType) {
            const auto& [value,currentType] = operand;

            if(currentType == targetType) return operand;

            if(currentType == "boolean"){
                bool val = std::any_cast<bool>(value);
                if(targetType == "int") return {static_cast<int>(val),"int"};
                if(targetType == "decimal") return {static_cast<double>(val),"decimal"};
            }
            if(currentType == "int"){
                int val = std::any_cast<int>(value);
                if(targetType == "decimal") return {static_cast<double>(val),"decimal"};
            }

            std::cerr<<"Promotion invalid";
            return {"nil","nil"};
        }

        std::string stringify(LiteralValue result) const {
            if (result.second == "nil") return "nil";

            std::string text;

            if (result.second == "boolean") {
                text = std::any_cast<bool>(result.first) ? "true" : "false";
            } else if (result.second == "int") {
                text = std::to_string(std::any_cast<int>(result.first));
            } else if (result.second == "decimal") {
                text = std::to_string(std::any_cast<double>(result.first));
            } else if (result.second == "string") {
                text = std::any_cast<std::string>(result.first);
            } else {
                text = "nil";
            }

            return text;
        }

        LiteralValue visitLiteralExpr(LiteralExpr& expr) override {
            return expr.literal;
        }

        LiteralValue visitGroupingExpr(GroupingExpr& expr) override {
            return evaluate(expr.expression);
        }

        LiteralValue visitUnaryExpr(UnaryExpr& expr) override {
            LiteralValue right = evaluate(expr.right);

            switch(expr.Operator.type){
                case TokenType::MINUS : 
                    if(right.second == "int")
                        return  {- std::any_cast<int>(right.first),"int"};
                    if(right.second == "decimal")
                        return {- std::any_cast<double>(right.first),"decimal"};
                    throw RuntimeError(expr.Operator,"Unsupported operand");
                case TokenType::BANG:
                    return {!std::any_cast<bool>(isTruthy(right).first),"boolean"};
            }

            return {"nil","nil"};
        }

        LiteralValue visitVariableExpr(VariableExpr& expr) override {
            return environment->get(expr.name);
        }

        LiteralValue visitBinaryExpr(BinaryExpr& expr) override {
           LiteralValue left = evaluate(expr.left);
           LiteralValue right = evaluate(expr.right);

           std::string targetType = getPriority(left.second) > getPriority(right.second) ? left.second : right.second;
           
           auto [leftval,leftType] = promoteType(left,targetType);
           auto [rightval,rightType] = promoteType(right,targetType);
           switch(expr.Operator.type){
               case TokenType::GREATER:
                    if(targetType == "int") return {std::any_cast<int>(leftval) > std::any_cast<int>(rightval), "int"};
                    if(targetType == "decimal") return {std::any_cast<double>(leftval) > std::any_cast<double>(rightval), "decimal"};
                case TokenType::GREATER_EQUAL:
                    if(targetType == "int") return {std::any_cast<int>(leftval) >= std::any_cast<int>(rightval), "int"};
                    if(targetType == "decimal") return {std::any_cast<double>(leftval) >= std::any_cast<double>(rightval), "decimal"};
                case TokenType::LESS:
                    if(targetType == "int") return {std::any_cast<int>(leftval) < std::any_cast<int>(rightval), "int"};
                    if(targetType == "decimal") return {std::any_cast<double>(leftval) < std::any_cast<double>(rightval), "decimal"};
                case TokenType::LESS_EQUAL:
                     if(targetType == "int") return {std::any_cast<int>(leftval) <= std::any_cast<int>(rightval), "int"};
                    if(targetType == "decimal") return {std::any_cast<double>(leftval) <= std::any_cast<double>(rightval), "decimal"};
               case TokenType::BANG_EQUAL:
                     if(targetType == "int") return {std::any_cast<int>(leftval) != std::any_cast<int>(rightval), "int"};
                    if(targetType == "decimal") return {std::any_cast<double>(leftval) != std::any_cast<double>(rightval), "decimal"};
               case TokenType::EQUAL_EQUAL:
                    return isEqual(left,right); break;
                case TokenType::PLUS:
                    if(targetType == "int") return {std::any_cast<int>(leftval) + std::any_cast<int>(rightval), "int"};
                    if(targetType == "decimal") return {std::any_cast<double>(leftval) + std::any_cast<double>(rightval), "decimal"};
                    if(left.second == "string" && right.second == "string") return {std::any_cast<std::string>(left.first) + std::any_cast<std::string>(right.first),"string"};
                   throw RuntimeError(expr.Operator,"Operands are of incompatible types!");
                   break;
                case TokenType::MINUS:
                    if(targetType == "int") return {std::any_cast<int>(leftval) - std::any_cast<int>(rightval), "int"};
                    if(targetType == "decimal") return {std::any_cast<double>(leftval) - std::any_cast<double>(rightval), "decimal"};
               case TokenType::STAR:
                    if(targetType == "int") return {std::any_cast<int>(leftval) * std::any_cast<int>(rightval), "int"};
                    if(targetType == "decimal") return {std::any_cast<double>(leftval) * std::any_cast<double>(rightval), "decimal"};
               case TokenType::SLASH:
                    if(targetType == "int"){
                        if(std::any_cast<int>(rightval) != 0)
                            return {std::any_cast<int>(leftval) > std::any_cast<int>(rightval), "int"};
                        throw RuntimeError(expr.Operator,"Divide by zero error");
                    }
                    if(targetType == "decimal"){
                        if(std::any_cast<double>(rightval) != 0.0)
                            return {std::any_cast<double>(leftval) > std::any_cast<double>(rightval), "int"};
                        throw RuntimeError(expr.Operator,"Divide by zero error");
                    }
               case TokenType::PERCENT:
                    if(targetType == "int") return {std::any_cast<int>(leftval) % std::any_cast<int>(rightval), "int"};
                    if(targetType == "decimal") return {fmod(std::any_cast<double>(leftval),std::any_cast<double>(rightval)), "decimal"};
          }

           return {"nil","nil"};
        }

        LiteralValue visitAssignExpr(AssignExpr& expr) override {
            LiteralValue value = evaluate(expr.value);
            environment->assign(expr.name,value);
            return value;
        }

        LiteralValue visitPrintStmt(PrintStmt& statement) override {
            LiteralValue value = evaluate(statement.expression);
            std::cout<<stringify(value);
            return {"nil","nil"};
        }

        LiteralValue visitExprStmt(ExprStmt& statement) override{
            evaluate(statement.expression);
            return {"nil","nil"};
        }

        LiteralValue visitVarStmt(VarStmt& statement) override {
            LiteralValue value;
            if(statement.initializer != nullptr){
                value = evaluate(statement.initializer);
            }

            environment->define(statement.name.lexeme,value,statement.type.lexeme);
            return {"nil","nil"};
        }

        LiteralValue visitIfStmt(IfStmt& statement) override {
            if(std::any_cast<bool>(isTruthy(evaluate(statement.ifCondition)).first)){
                execute(statement.thenBranch);
            } else if(statement.elifCondition != nullptr && std::any_cast<bool>(isTruthy(evaluate(statement.elifCondition)).first)){
                execute(statement.elifBranch);
            } else if(statement.elseBranch != nullptr){
                execute(statement.elseBranch);
            }
            return {"nil","nil"};
        }

        LiteralValue visitBlockStmt(BlockStmt& stmt) override {
            Environment* newEnvironment = new Environment(environment);
            executeBlock(stmt.statements, newEnvironment);
            return {"nil","nil"};
        }

    public:

        void interpret(std::vector<Statement>& statements){
            try{
                for(auto& statement : statements) {
                    execute(statement);
                }
            } catch(RuntimeError& err){
                Adharma::runtimeError(err);
            }
        }
};
