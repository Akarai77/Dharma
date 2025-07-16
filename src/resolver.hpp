#pragma once

#include "error.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"
#include <cmath>
#include <unordered_map>
#include <vector>
#include <iostream> // For logging

enum class FunctionType {
    NONE,
    FUNCTION
};

class Resolver : public ExprVisitor, public StmtVisitor {
    private:
        Interpreter& interpreter;
        std::vector<std::unordered_map<std::string, bool>> scopes;
        FunctionType currentFunction = FunctionType::NONE;

        void resolve(const Statement& stmt) {
            stmt->accept(*this);
        }

        void resolve(const Expression& expr) {
            expr->accept(*this);
        }

        void beginScope() {
            scopes.emplace_back();
        }

        void endScope() {
            scopes.pop_back();
        }

        void declare(const Token& name) {
            if (scopes.empty()) return;
            auto& scope = scopes.back();
            if (scope.contains(name.lexeme)) {
                throw ParseError(name, "Already a variable with this name in the scope.");
            }
            scope[name.lexeme] = false;
        }

        void define(const Token& name) {
            if (!scopes.empty()) {
                scopes.back()[name.lexeme] = true;
            }
        }

        void resolveLocal(Expr* expr,const Token& name) {
            for(int i = scopes.size() - 1; i >= 0; i--) {
                if(scopes[i].contains(name.lexeme)) {
                    int depth = scopes.size() - 1 - i;
                    interpreter.resolve(expr, depth);
                    return;
                }
            }
        }

        void resolveFunction(const FunctionStmt& function, FunctionType type) {
            FunctionType enclosingFunction = currentFunction;
            currentFunction = type;

            beginScope();
            for(auto& stmt : function.params) {
                if(auto param = dynamic_cast<VarStmt*>(stmt.get())) {
                    declare(param->name);
                    define(param->name);
                }
            }
            resolve(function.body);
            endScope();
            currentFunction = enclosingFunction;
        }

    public:

        Resolver(Interpreter& interpreter) : interpreter(interpreter) {}

        void resolve(const std::vector<Statement>& statements) {
            try {
                for(auto& statement : statements) {
                    resolve(statement);
                }
            } catch(ParseError& err) {
                std::cout << "[Resolver] ParseError: " << err.message() << "\n";
            }
        }

        RuntimeValue visitBlockStmt(BlockStmt& stmt) override {
            beginScope();
            resolve(stmt.statements);
            endScope();
            return _NIL;
        }

        RuntimeValue visitClassStmt(ClassStmt& stmt) override {
            declare(stmt.name);
            define(stmt.name);
            return _NIL;
        }

        RuntimeValue visitExprStmt(ExprStmt& stmt) override {
            resolve(stmt.expression);
            return _NIL;
        }
        
        RuntimeValue visitFunctionStmt(FunctionStmt& stmt) override {
            declare(stmt.name);
            define(stmt.name);

            resolveFunction(stmt,FunctionType::FUNCTION);
            return _NIL;
        }

        RuntimeValue visitIfStmt(IfStmt& stmt) override {
            resolve(stmt.ifCondition);
            resolve(stmt.thenBranch);
            if(stmt.elifCondition != nullptr) {
                resolve(stmt.elifCondition);
                resolve(stmt.elifBranch);
            }
            if(stmt.elseBranch != nullptr) {
                resolve(stmt.elseBranch);
            }
            return _NIL;
        }

        RuntimeValue visitPrintStmt(PrintStmt& stmt) override {
            resolve(stmt.expression);
            return _NIL;
        }

        RuntimeValue visitReturnStmt(ReturnStmt& stmt) override {
            if(currentFunction == FunctionType::NONE) {
                throw ParseError(stmt.keyword,"Cannot return from top-level code.");
            }

            if(stmt.value != nullptr) {
                resolve(stmt.value);
            }

            return _NIL;
        }

        RuntimeValue visitVarStmt(VarStmt& stmt) override {
            declare(stmt.name);
            if(stmt.initializer != nullptr) {
                resolve(stmt.initializer);
            }
            define(stmt.name);
            return _NIL;
        }

        RuntimeValue visitWhileStmt(WhileStmt& stmt) override {
            resolve(stmt.condition);
            resolve(stmt.body);

            return _NIL;
        }

        RuntimeValue visitForStmt(ForStmt& stmt) override {
            if(stmt.initializer != nullptr) resolve(stmt.initializer);
            if(stmt.condition != nullptr) resolve(stmt.condition);
            if(stmt.increment != nullptr) resolve(stmt.increment);
            resolve(stmt.body);
            return _NIL;
        }

        RuntimeValue visitGetExpr(GetExpr& expr) override {
            resolve(expr.object);
            return _NIL;
        }

        RuntimeValue visitSetExpr(SetExpr& expr) override {
            resolve(expr.value);
            resolve(expr.object);
            return _NIL;
        }

        RuntimeValue visitVariableExpr(VariableExpr& expr) override {
            if (!scopes.empty() && scopes.back().contains(expr.name.lexeme) && !scopes.back()[expr.name.lexeme]) {
                throw ParseError(expr.name, "Cannot read local variable in its own initializer.");
            }
            resolveLocal(&expr, expr.name);
            return _NIL;
        }

        RuntimeValue visitAssignExpr(AssignExpr& expr) override {
            resolve(expr.value);
            resolveLocal(&expr, expr.name);
            return _NIL;
        }

        RuntimeValue visitBinaryExpr(BinaryExpr& expr) override {
            resolve(expr.left);
            resolve(expr.right);

            return _NIL;
        }

        RuntimeValue visitCallExpr(CallExpr& expr) override {
            resolve(expr.callee);

            for(auto& argument : expr.arguments) {
                resolve(argument);
            }

            return _NIL;
        }

        RuntimeValue visitGroupingExpr(GroupingExpr& expr) override {
            resolve(expr.expression);
            return _NIL;
        }

        RuntimeValue visitLiteralExpr(LiteralExpr& expr) override {
            return _NIL;
        }

        RuntimeValue visitLogicalExpr(LogicalExpr& expr) override {
            resolve(expr.left);
            resolve(expr.right);
            return _NIL;
        }

        RuntimeValue visitUnaryExpr(UnaryExpr& expr) override {
            resolve(expr.right);
            return _NIL;
        }
};

