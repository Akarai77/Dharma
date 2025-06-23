#pragma once

#include "expr.hpp"
#include <vector>
#include <optional>
#include <memory>

class Stmt;
class BlockStmt;
class ExprStmt;
class PrintStmt;
class VarStmt;

using Statement = std::unique_ptr<Stmt>;

template <typename T, typename... Args>
std::unique_ptr<T> makeStmt(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

class StmtVisitor {
public:
	virtual LiteralValue visitBlockStmt(BlockStmt& stmt) = 0;
	virtual LiteralValue visitExprStmt(ExprStmt& stmt) = 0;
	virtual LiteralValue visitPrintStmt(PrintStmt& stmt) = 0;
	virtual LiteralValue visitVarStmt(VarStmt& stmt) = 0;
	virtual ~StmtVisitor() = default;
};

class Stmt {
public:
	virtual LiteralValue accept(StmtVisitor& visitor) = 0;
	virtual ~Stmt() = default;
};

class BlockStmt : public Stmt {
public:
	std::vector<Statement> statements;

	BlockStmt(std::vector<Statement> statements) : statements(std::move(statements)) {}
	LiteralValue accept(StmtVisitor& visitor) override {
		return visitor.visitBlockStmt(*this);
	}
};

class ExprStmt : public Stmt {
public:
	Expression expression;

	ExprStmt(Expression expression) : expression(std::move(expression)) {}
	LiteralValue accept(StmtVisitor& visitor) override {
		return visitor.visitExprStmt(*this);
	}
};

class PrintStmt : public Stmt {
public:
	Expression expression;

	PrintStmt(Expression expression) : expression(std::move(expression)) {}
	LiteralValue accept(StmtVisitor& visitor) override {
		return visitor.visitPrintStmt(*this);
	}
};

class VarStmt : public Stmt {
public:
	Token name;
	std::optional<Token> type;
	Expression initializer;

	VarStmt(Token name, std::optional<Token> type, Expression initializer) : name(name), type(type), initializer(std::move(initializer)) {}
	LiteralValue accept(StmtVisitor& visitor) override {
		return visitor.visitVarStmt(*this);
	}
};
