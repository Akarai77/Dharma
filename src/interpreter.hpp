#pragma once

#include "function.hpp"
#include "token.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "tokenType.hpp"
#include "environment.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include "error.hpp"
#include "util.hpp"
#include "callable.hpp"
#include "warning.hpp"

#define BIN_OP(actualType, op, retType) \
    return LiteralValue{std::get<actualType>(leftval) op std::get<actualType>(rightval), retType};

#define TYPE_BIN_OP(type, actualType, op, retType) \
    if(targetType == type) BIN_OP(actualType, op, retType);

class Interpreter : public ExprVisitor, public StmtVisitor{
    private:

        bool isTruthy(LiteralValue literal) const {
            if(literal.second == "nil") return false;
            if(literal.second == "boolean") return std::get<bool>(literal.first);
            return true;
        }

        LiteralValue isEqual(const LiteralCore& l,const LiteralCore& r) const {
            if(isNil(l) && isNil(r)) return {true,"bool"};
            if(isNil(l)) return {false,"bool"};

            return {true,"boolean"};
        }

        bool isLiteral(Expr* expr) {
            return dynamic_cast<LiteralExpr*>(expr) != nullptr;
        }

        int getPriority(std::string type){
            if(type == "boolean") return 0;
            if(type == "integer") return 1;
            if(type == "decimal") return 2;
            if(type == "BigDecimal") return 3;
            if(type == "string") return 4;
            return -1;
        }

        LiteralValue promoteType(const LiteralValue& operand,std::string targetType,const Token& token,RuntimeError err) {
            const auto& [value,currentType] = operand;

            if(currentType == targetType) return operand;
            std::cout<<ImplicitConversionWarning(token,targetType,currentType).message();
            if(currentType == "nil"){
                if(targetType == "integer") return {Integer(0),targetType};
                if(targetType == "decimal") return {static_cast<double>(0),targetType};
                if(targetType == "BigDecimal") return {BigDecimal(0),targetType};
                if(targetType == "boolean") return {false,"boolean"};
            }
            if(currentType == "boolean"){
                bool val = std::get<bool>(value);
                if(targetType == "integer") return {Integer(val),"integer"};
                if(targetType == "decimal") return {static_cast<double>(val),"decimal"};
                if(targetType == "BigDecimal") return {BigDecimal(val),"BigDecimal"};
            }
            if(currentType == "integer"){
                Integer val = std::get<Integer>(value);
                if(targetType == "decimal") return {val.toDecimal(),"decimal"};
                if(targetType == "BigDecimal") return {val.toBigDecimal(),"BigDecimal"};
            }
            if(currentType == "decimal"){
                double val = std::get<double>(value);
                if(targetType == "BigDecimal") return {BigDecimal(val),"BigDecimal"};
            }
            
            throw err;
        }

        std::string stringify(LiteralValue result) const {
            std::string text;

            if (result.second == "boolean") {
                text = std::get<bool>(result.first) ? "true" : "false";
            } else if (result.second == "integer") {
                text = (std::get<Integer>(result.first)).toString();
            } else if (result.second == "decimal") {
                text = cleanDouble(std::get<double>(result.first));
            } else if(result.second == "BigDecimal") {
                text = (std::get<BigDecimal>(result.first)).toString();
            } else if (result.second == "string") {
                text = "'"+std::get<std::string>(result.first)+"'";
            } else {
                text = "nil";
            }

            return text;
        }

        RuntimeValue visitLiteralExpr(LiteralExpr& expr) override {
            return expr.literal;
        }

        RuntimeValue visitGroupingExpr(GroupingExpr& expr) override {
            return evaluate(expr.expression);
        }

        RuntimeValue visitUnaryExpr(UnaryExpr& expr) override {
            LiteralValue value = getLiteralValue(evaluate(expr.right));

            switch(expr.Operator.type){
                case TokenType::MINUS : 
                    if(value.second == "integer")
                        return  LiteralValue{- std::get<Integer>(value.first),"integer"};
                    if(value.second == "decimal")
                        return LiteralValue{- std::get<double>(value.first),"decimal"};
                    if(value.second == "BigDecimal")
                        return LiteralValue{- std::get<BigDecimal>(value.first),"BigDecimal"};
                    throw RuntimeError(expr.Operator,"Unsupported operand");

                case TokenType::BANG :
                    return LiteralValue{!isTruthy(value),"boolean"};

                case TokenType::PRE_INCR :
                case TokenType::POST_INCR : 
                    {
                        LiteralValue original = value;
                        LiteralValue incrementedValue;

                        if(value.second == "integer")
                            incrementedValue = {++std::get<Integer>(value.first),"integer"};
                        else if(value.second == "decimal")
                            incrementedValue = {std::get<double>(value.first)+1,"decimal"};
                        else if(value.second == "BigDecimal")
                            incrementedValue = {++std::get<BigDecimal>(value.first),"decimal"};
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

                        if(value.second == "integer")
                            decrementedValue = {--std::get<Integer>(value.first),"integer"};
                        else if(value.second == "decimal")
                            decrementedValue = {std::get<double>(value.first)-1,"decimal"};
                        else if(value.second == "BigDecimal")
                            decrementedValue = {--std::get<BigDecimal>(value.first),"BigDecimal"};
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

            return LiteralValue{Nil(),"nil"};
        }

        RuntimeValue visitVariableExpr(VariableExpr& expr) override {
            return environment->get(expr.name);
        }

        RuntimeValue visitBinaryExpr(BinaryExpr& expr) override {
           LiteralValue left = getLiteralValue(evaluate(expr.left));
           LiteralValue right = getLiteralValue(evaluate(expr.right));

           std::string targetType = getPriority(left.second) > getPriority(right.second) ? left.second : right.second;
           auto [leftval,leftType] = promoteType(left,targetType,expr.Operator,RuntimeError(expr.Operator,"Operands are of incompatible types!"));
           auto [rightval,rightType] = promoteType(right,targetType,expr.Operator,RuntimeError(expr.Operator,"Operands are of incompatible types!"));
           switch(expr.Operator.type){
               
               case TokenType::GREATER:
                    TYPE_BIN_OP("integer",Integer,>,"boolean");
                    TYPE_BIN_OP("decimal",double,>,"boolean");
                    TYPE_BIN_OP("BigDecimal",BigDecimal,>,"boolean")
                    TYPE_BIN_OP("string",std::string,>,"boolean");
                    TYPE_BIN_OP("boolean",bool,>,"boolean");
                    break;
                case TokenType::GREATER_EQUAL:
                    TYPE_BIN_OP("integer",Integer,>=,"boolean");
                    TYPE_BIN_OP("decimal",double,>=,"boolean");
                    TYPE_BIN_OP("BigDecimal",BigDecimal,>=,"boolean")
                    TYPE_BIN_OP("string",std::string,>=,"boolean");
                    TYPE_BIN_OP("boolean",bool,>=,"boolean");
                    break;
                case TokenType::LESS:
                    TYPE_BIN_OP("integer",Integer,<,"boolean");
                    TYPE_BIN_OP("decimal",double,<,"boolean");
                    TYPE_BIN_OP("BigDecimal",BigDecimal,<,"boolean")
                    TYPE_BIN_OP("string",std::string,<,"boolean");
                    TYPE_BIN_OP("boolean",bool,<,"boolean");
                    break;
                case TokenType::LESS_EQUAL:
                    TYPE_BIN_OP("integer",Integer,<=,"boolean");
                    TYPE_BIN_OP("decimal",double,<=,"boolean");
                    TYPE_BIN_OP("BigDecimal",BigDecimal,<=,"boolean")
                    TYPE_BIN_OP("string",std::string,<=,"boolean");
                    TYPE_BIN_OP("boolean",bool,<=,"boolean");
                    break;
               case TokenType::BANG_EQUAL:
                    TYPE_BIN_OP("integer",Integer,!=,"boolean");
                    TYPE_BIN_OP("decimal",double,!=,"boolean");
                    TYPE_BIN_OP("BigDecimal",BigDecimal,!=,"boolean")
                    TYPE_BIN_OP("string",std::string,!=,"boolean");
                    TYPE_BIN_OP("boolean",bool,!=,"boolean");
                    break;
               case TokenType::EQUAL_EQUAL:
                    TYPE_BIN_OP("integer",Integer,==,"boolean");
                    TYPE_BIN_OP("decimal",double,==,"boolean");
                    TYPE_BIN_OP("BigDecimal",BigDecimal,==,"boolean")
                    TYPE_BIN_OP("string",std::string,==,"boolean");
                    TYPE_BIN_OP("boolean",bool,==,"boolean");
                    break;
               case TokenType::PLUS:
                    TYPE_BIN_OP("integer",Integer,+,"integer");
                    TYPE_BIN_OP("decimal",double,+,"decimal");
                    TYPE_BIN_OP("BigDecimal",BigDecimal,+,"BigDecimal")
                    TYPE_BIN_OP("boolean",Integer,+,"integer");
                    TYPE_BIN_OP("string",std::string,+,"string");
                    break;
               case TokenType::MINUS:
                    TYPE_BIN_OP("integer",Integer,-,"integer");
                    TYPE_BIN_OP("decimal",double,-,"decimal");
                    TYPE_BIN_OP("BigDecimal",BigDecimal,-,"BigDecimal")
                    TYPE_BIN_OP("boolean",Integer,-,"integer");
                    if(targetType == "string") throw RuntimeError(expr.Operator,"Unsupported operand type for 'string' and 'string'.");
                    break;
               case TokenType::STAR:
                    TYPE_BIN_OP("integer",Integer,*,"integer");
                    TYPE_BIN_OP("decimal",double,*,"decimal");
                    TYPE_BIN_OP("BigDecimal",BigDecimal,*,"BigDecimal")
                    TYPE_BIN_OP("boolean",Integer,*,"integer");
                    if(targetType == "string") throw RuntimeError(expr.Operator,"Unsupported operand type for 'string' and 'string'.");
                    break;
               case TokenType::SLASH:
                    if(targetType == "integer"){
                        if(std::get<Integer>(rightval) != 0) BIN_OP(Integer,/,"integer");
                        throw RuntimeError(expr.Operator,"Divide by zero error");
                    }
                    if(targetType == "decimal"){
                        if(std::get<double>(rightval) != 0.0) BIN_OP(double,/,"decimal");
                        throw RuntimeError(expr.Operator,"Divide by zero error");
                    }
                    if(targetType == "BigDecimal"){
                        if(std::get<BigDecimal>(rightval) != 0) BIN_OP(BigDecimal,/,"BigDecimal");
                        throw RuntimeError(expr.Operator,"Divide by zero error");
                    }
                    if(targetType == "boolean"){
                        if(std::get<bool>(rightval) != false) BIN_OP(Integer,/,"integer");
                        throw RuntimeError(expr.Operator,"Divide by zero error ('false' evaluates to '0')");
                    }
                    if(targetType == "string") throw RuntimeError(expr.Operator,"Unsupported operand type for 'string' and 'string'.");
                    break;
               case TokenType::PERCENT:
                    if(targetType == "integer"){
                        if(std::get<Integer>(rightval) != 0) BIN_OP(Integer,%,"integer");
                        throw RuntimeError(expr.Operator,"Modulo by zero error");
                    }
                    if(targetType == "decimal"){
                        if(std::get<double>(rightval) != 0.0) 
                            return LiteralValue{fmod(std::get<double>(leftval),std::get<double>(rightval)),"decimal"};
                        throw RuntimeError(expr.Operator,"Modulo by zero error");
                    }
                    if(targetType == "BigDecimal"){
                        if(std::get<BigDecimal>(rightval) != 0) BIN_OP(BigDecimal,%,"BigDecimal");
                        throw RuntimeError(expr.Operator,"Modulo by zero error");
                    }
                    if(targetType == "boolean"){
                        if(std::get<bool>(rightval) != false) BIN_OP(Integer,%,"integer");
                        throw RuntimeError(expr.Operator,"Modulo by zero error ('false' evaluates to '0')");
                    }
                    if(targetType == "string") throw RuntimeError(expr.Operator,"Unsupported operand type for 'string' and 'string'.");
                    break;
          }

           return LiteralValue{Nil(),"nil"};
        }

        RuntimeValue visitCallExpr(CallExpr& expr) override {
            RuntimeValue callee = evaluate(expr.callee);

            std::vector<LiteralValue> arguments;
            for(auto& argument : expr.arguments) {
                arguments.push_back(getLiteralValue(evaluate(argument)));
            }

            if (auto function = std::get_if<std::shared_ptr<Callable>>(&callee)) {
                int arity = (*function)->arity();
                if(arity != arguments.size()) {
                    throw RuntimeError(expr.paren,"Expected "+std::to_string(arity)+" arguments but got "+std::to_string(arguments.size())+".");
                }
                return (*function)->call(*this, arguments);
            } else {
                throw RuntimeError(expr.paren,"Can only call functions and classes.");
            }
            
            return LiteralValue{Nil(),"nil"};
        }

        RuntimeValue visitLogicalExpr(LogicalExpr& expr) override {
            LiteralValue left = getLiteralValue(evaluate(expr.left));

            switch (expr.Operator.type) {
                case TokenType::OR:
                    if (isTruthy(left)) return left;
                    return evaluate(expr.right);

                case TokenType::AND:
                    if (!isTruthy(left)) return left;
                    return evaluate(expr.right);

                case TokenType::PIPE_PIPE:
                    if (isTruthy(left)) return LiteralValue{ true, "boolean" };
                    return LiteralValue{ isTruthy(getLiteralValue(evaluate(expr.right))), "boolean" };

                case TokenType::AMP_AMP:
                    if (!isTruthy(left)) return LiteralValue{ false, "boolean" };
                    return LiteralValue{ isTruthy(getLiteralValue(evaluate(expr.right))), "boolean" };

                default:
                    throw RuntimeError(expr.Operator,"Invalid logical operator.");
            }
        }

        RuntimeValue visitAssignExpr(AssignExpr& expr) override {
            LiteralValue value = getLiteralValue(evaluate(expr.value));
            environment->assign(expr.name,value);
            return value;
        }

        RuntimeValue visitPrintStmt(PrintStmt& statement) override {
            LiteralValue value = getLiteralValue(evaluate(statement.expression));
            std::cout<<stringify(value)<<std::endl;
            return LiteralValue{Nil(),"nil"};
        }

        RuntimeValue visitExprStmt(ExprStmt& statement) override{
            evaluate(statement.expression);
            return LiteralValue{Nil(),"nil"};
        }

        RuntimeValue visitVarStmt(VarStmt& statement) override {
            RuntimeValue value = LiteralValue{Nil(),"nil"};
            if(statement.initializer != nullptr){
                value = evaluate(statement.initializer);
            }

            environment->define(statement.name.lexeme,value,statement.type.lexeme);
            return LiteralValue{Nil(),"nil"};
        }

        RuntimeValue visitFunctionStmt(FunctionStmt& statement) override {
            Function function(statement);
            environment->define(statement.name.lexeme,std::make_shared<Function>(function),statement.kind);
            return LiteralValue{Nil(),"nil"};
        }

        RuntimeValue visitIfStmt(IfStmt& statement) override {
            if(isTruthy(getLiteralValue(evaluate(statement.ifCondition)))){
                execute(statement.thenBranch);
            } else if(statement.elifCondition != nullptr && isTruthy(getLiteralValue(evaluate(statement.elifCondition)))){
                execute(statement.elifBranch);
            } else if(statement.elseBranch != nullptr){
                execute(statement.elseBranch);
            }
            return LiteralValue{Nil(),"nil"};
        }

        RuntimeValue visitWhileStmt(WhileStmt& statement) override {
            while(isTruthy(getLiteralValue(evaluate(statement.condition)))){
                execute(statement.body);
            }

            return LiteralValue{Nil(),"nil"};
        }

        RuntimeValue visitForStmt(ForStmt& statement) override {
            if (statement.initializer != nullptr) {
                execute(statement.initializer);
            }

            while (true) {
                if (statement.condition != nullptr && !isTruthy(getLiteralValue(evaluate(statement.condition)))) {
                    break;
                }

                execute(statement.body);

                if (statement.increment != nullptr) {
                    evaluate(statement.increment);
                }
            }

            return LiteralValue{Nil(), "nil"};
        }

        RuntimeValue visitBlockStmt(BlockStmt& stmt) override {
            Environment newEnvironment;
            executeBlock(stmt.statements, newEnvironment);
            return LiteralValue{Nil(),"nil"};
        }

    public:
        Environment globals;
        Environment environment;

        Interpreter() : globals(std::make_shared<Env>()), environment(globals) {
            globals->define("clock",std::make_shared<ClockFunction>(),"function");
        }

        ~Interpreter() {}

        void interpret(std::vector<Statement>& statements){
            try{
                for(auto& statement : statements) {
                    execute(statement);
                }
            } catch(RuntimeError& err){
                std::cerr<<err.message();
            }
        }

        RuntimeValue evaluate(Expression& expression){
            return expression->accept(*this);
        }

        void execute(Statement& statement){
            statement->accept(*this);
        }

        void executeBlock(std::vector<Statement>& statements,Environment newEnvironment){
            EnvSwitch Switch(this->environment,newEnvironment);

            for(auto& statement : statements){
                execute(statement);
            }
        }

};

LiteralValue Function::call(Interpreter& interpreter,const std::vector<LiteralValue>& args) {
    Environment environment(interpreter.globals);

    for(int i = 0;i<declaration.params.size();i++){
        environment->define(declaration.params[i].lexeme,args[i],args[i].second);
    }

    interpreter.executeBlock(declaration.body,std::move(environment));
    return {Nil(),"nil"};
}
