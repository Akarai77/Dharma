#pragma once

#include "Token.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "tokenType.hpp"
#include "environment.hpp"
#include <any>
#include <cmath>
#include <string>
#include <vector>
#include "error.hpp"

#define BIN_OP(castType, op, retType) \
    return {std::any_cast<castType>(leftval) op std::any_cast<castType>(rightval), retType};

#define TYPE_BIN_OP(type, castType, op, retType) \
    if(targetType == type) BIN_OP(castType, op, retType);

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

        bool isTruthy(LiteralValue literal) const {
            if(literal.second == "nil") return false;
            if(literal.second == "boolean") return std::any_cast<bool>(literal.first);
            return true;
        }

        LiteralValue isEqual(const std::any& l,const std::any& r) const {
            if(!l.has_value() && !r.has_value()) return {true,"bool"};
            if(!l.has_value()) return {false,"bool"};

            return {true,"boolean"};
        }

        bool isLiteral(Expr* expr) {
            return dynamic_cast<LiteralExpr*>(expr) != nullptr;
        }

        int getPriority(std::string type){
            if(type == "boolean") return 0;
            if(type == "int") return 1;
            if(type == "decimal") return 2;
            if(type == "string") return 3;
            return -1;
        }

        LiteralValue promoteType(const LiteralValue& operand,std::string targetType,RuntimeError err) {
            const auto& [value,currentType] = operand;

            if(currentType == targetType) return operand;
            //warn nil->othertype
            if(currentType == "nil"){
                if(targetType == "int" || targetType == "decimal") return {0,targetType};
                if(targetType == "boolean") return {false,"boolean"};
            }
            if(currentType == "boolean"){
                bool val = std::any_cast<bool>(value);
                if(targetType == "int") return {static_cast<int>(val),"int"};
                if(targetType == "decimal") return {static_cast<double>(val),"decimal"};
            }
            if(currentType == "int"){
                int val = std::any_cast<int>(value);
                if(targetType == "decimal") return {static_cast<double>(val),"decimal"};
            }
            
            throw err;
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
                text = "'"+std::any_cast<std::string>(result.first)+"'";
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
            LiteralValue value = evaluate(expr.right);

            switch(expr.Operator.type){
                case TokenType::MINUS : 
                    if(value.second == "int")
                        return  {- std::any_cast<int>(value.first),"int"};
                    if(value.second == "decimal")
                        return {- std::any_cast<double>(value.first),"decimal"};
                    throw RuntimeError(expr.Operator,"Unsupported operand");

                case TokenType::BANG :
                    return {!isTruthy(value),"boolean"};

                case TokenType::PRE_INCR :
                case TokenType::POST_INCR : 
                    {
                        LiteralValue original = value;
                        LiteralValue incrementedValue;

                        if(value.second == "int")
                            incrementedValue = {std::any_cast<int>(value.first)+1,"int"};
                        else if(value.second == "decimal")
                            incrementedValue = {std::any_cast<double>(value.first)+1,"decimal"};
                        else
                            throw RuntimeError(expr.Operator, "Invalid operand type for '++'");

                        if(auto varExpr = dynamic_cast<VariableExpr*>(expr.right.get())) {
                            environment->assign(varExpr->name,incrementedValue);

                            return (expr.Operator.type == TokenType::PRE_INCR)
                                ? incrementedValue
                                : original;
                        } else if(isLiteral(expr.right.get())) {
                            throw RuntimeError(expr.Operator,"Cannot apply '++' to a literal.");
                        } else {
                            throw RuntimeError(expr.Operator,"Cannot apply '++' to non-assignable expression.");
                        }
                    }

                case TokenType::PRE_DECR :
                case TokenType::POST_DECR :
                    {
                        LiteralValue original = value;
                        LiteralValue decrementedValue;

                        if(value.second == "int")
                            decrementedValue = {std::any_cast<int>(value.first)-1,"int"};
                        else if(value.second == "decimal")
                            decrementedValue = {std::any_cast<double>(value.first)-1,"decimal"};
                        else
                            throw RuntimeError(expr.Operator, "Invalid operand type for '--'");

                        if(auto varExpr = dynamic_cast<VariableExpr*>(expr.right.get())) {
                            environment->assign(varExpr->name,decrementedValue);

                            return (expr.Operator.type == TokenType::PRE_DECR)
                                ? decrementedValue
                                : original;
                        } else if(isLiteral(expr.right.get())) {
                            throw RuntimeError(expr.Operator,"Cannot apply '--' to a literal");
                        } else {
                            throw RuntimeError(expr.Operator,"Cannot apply '--' to non-assignable expression.");
                        }
                    }

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
           auto [leftval,leftType] = promoteType(left,targetType,RuntimeError(expr.Operator,"Operands are of incompatible types!"));
           auto [rightval,rightType] = promoteType(right,targetType,RuntimeError(expr.Operator,"Operands are of incompatible types!"));
           switch(expr.Operator.type){
               
               case TokenType::GREATER:
                    TYPE_BIN_OP("int",int,>,"boolean");
                    TYPE_BIN_OP("decimal",double,>,"boolean");
                    TYPE_BIN_OP("string",std::string,>,"boolean");
                    TYPE_BIN_OP("boolean",bool,>,"boolean");
                    break;
                case TokenType::GREATER_EQUAL:
                    TYPE_BIN_OP("int",int,>=,"boolean");
                    TYPE_BIN_OP("decimal",double,>=,"boolean");
                    TYPE_BIN_OP("string",std::string,>=,"boolean");
                    TYPE_BIN_OP("boolean",bool,>=,"boolean");
                    break;
                case TokenType::LESS:
                    TYPE_BIN_OP("int",int,<,"boolean");
                    TYPE_BIN_OP("decimal",double,<,"boolean");
                    TYPE_BIN_OP("string",std::string,<,"boolean");
                    TYPE_BIN_OP("boolean",bool,<,"boolean");
                    break;
                case TokenType::LESS_EQUAL:
                    TYPE_BIN_OP("int",int,<=,"boolean");
                    TYPE_BIN_OP("decimal",double,<=,"boolean");
                    TYPE_BIN_OP("string",std::string,<=,"boolean");
                    TYPE_BIN_OP("boolean",bool,<=,"boolean");
                    break;
               case TokenType::BANG_EQUAL:
                    TYPE_BIN_OP("int",int,!=,"boolean");
                    TYPE_BIN_OP("decimal",double,!=,"boolean");
                    TYPE_BIN_OP("string",std::string,!=,"boolean");
                    TYPE_BIN_OP("boolean",bool,!=,"boolean");
                    break;
               case TokenType::EQUAL_EQUAL:
                    TYPE_BIN_OP("int",int,==,"boolean");
                    TYPE_BIN_OP("decimal",double,==,"boolean");
                    TYPE_BIN_OP("string",std::string,==,"boolean");
                    TYPE_BIN_OP("boolean",bool,==,"boolean");
                    break;

               case TokenType::PLUS:
                    TYPE_BIN_OP("int",int,+,"int");
                    TYPE_BIN_OP("decimal",double,+,"decimal");
                    TYPE_BIN_OP("boolean",bool,+,"int");
                    TYPE_BIN_OP("string",std::string,+,"string");
                    break;
               case TokenType::MINUS:
                    TYPE_BIN_OP("int",int,-,"int");
                    TYPE_BIN_OP("decimal",double,-,"decimal");
                    TYPE_BIN_OP("boolean",bool,-,"int");
                    if(targetType == "string") throw RuntimeError(expr.Operator,"Unsupported operand type for 'string' and 'string'.");
                    break;
               case TokenType::STAR:
                    TYPE_BIN_OP("int",int,*,"int");
                    TYPE_BIN_OP("decimal",double,*,"decimal");
                    TYPE_BIN_OP("boolean",bool,*,"int");
                    if(targetType == "string") throw RuntimeError(expr.Operator,"Unsupported operand type for 'string' and 'string'.");
                    break;
               case TokenType::SLASH:
                    if(targetType == "int"){
                        if(std::any_cast<int>(rightval) != 0) BIN_OP(int,/,"int");
                        throw RuntimeError(expr.Operator,"Divide by zero error");
                    }
                    if(targetType == "decimal"){
                        if(std::any_cast<double>(rightval) != 0.0) BIN_OP(double,/,"decimal");
                        throw RuntimeError(expr.Operator,"Divide by zero error");
                    }
                    if(targetType == "boolean"){
                        if(std::any_cast<bool>(rightval) != false) BIN_OP(bool,/,"int");
                        throw RuntimeError(expr.Operator,"Divide by zero error ('false' evaluates to '0')");
                    }
                    if(targetType == "string") throw RuntimeError(expr.Operator,"Unsupported operand type for 'string' and 'string'.");
                    break;
               case TokenType::PERCENT:
                    if(targetType == "int"){
                        if(std::any_cast<int>(rightval) != 0) BIN_OP(int,%,"int");
                        throw RuntimeError(expr.Operator,"Modulo by zero error");
                    }
                    if(targetType == "decimal"){
                        if(std::any_cast<double>(rightval) != 0.0) 
                            return {fmod(std::any_cast<double>(leftval),std::any_cast<double>(rightval)),"decimal"};
                        throw RuntimeError(expr.Operator,"Modulo by zero error");
                    }
                    if(targetType == "boolean"){
                        if(std::any_cast<bool>(rightval) != false) BIN_OP(bool,%,"int");
                        throw RuntimeError(expr.Operator,"Modulo by zero error ('false' evaluates to '0')");
                    }
                    if(targetType == "string") throw RuntimeError(expr.Operator,"Unsupported operand type for 'string' and 'string'.");
                    break;
          }

           return {"nil","nil"};
        }

        LiteralValue visitLogicalExpr(LogicalExpr& expr) override {
            LiteralValue left = evaluate(expr.left);

            switch (expr.Operator.type) {
                case TokenType::OR:
                    if (isTruthy(left)) return left;
                    return evaluate(expr.right);

                case TokenType::AND:
                    if (!isTruthy(left)) return left;
                    return evaluate(expr.right);

                case TokenType::PIPE_PIPE:
                    if (isTruthy(left)) return { true, "boolean" };
                    return { isTruthy(evaluate(expr.right)), "boolean" };

                case TokenType::AMP_AMP:
                    if (!isTruthy(left)) return { false, "boolean" };
                    return { isTruthy(evaluate(expr.right)), "boolean" };

                default:
                    throw std::runtime_error("Invalid logical operator.");
            }
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
            LiteralValue value = {"nil","nil"};
            if(statement.initializer != nullptr){
                value = evaluate(statement.initializer);
            }

            environment->define(statement.name.lexeme,value,statement.type.lexeme);
            return {"nil","nil"};
        }

        LiteralValue visitIfStmt(IfStmt& statement) override {
            if(isTruthy(evaluate(statement.ifCondition))){
                execute(statement.thenBranch);
            } else if(statement.elifCondition != nullptr && isTruthy(evaluate(statement.elifCondition))){
                execute(statement.elifBranch);
            } else if(statement.elseBranch != nullptr){
                execute(statement.elseBranch);
            }
            return {"nil","nil"};
        }

        LiteralValue visitWhileStmt(WhileStmt& statement) override {
            while(isTruthy(evaluate(statement.condition))){
                execute(statement.body);
            }

            return {"nil","nil"};
        }

        LiteralValue visitForStmt(ForStmt& statement) override {
            if (statement.initializer != nullptr) {
                execute(statement.initializer);
            }

            while (true) {
                if (statement.condition != nullptr && !isTruthy(evaluate(statement.condition))) {
                    break;
                }

                execute(statement.body);

                if (statement.increment != nullptr) {
                    evaluate(statement.increment);
                }
            }

            return {"nil", "nil"};
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
};;
