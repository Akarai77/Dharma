#pragma once

#include "Token.hpp"
#include <memory>

class Expr;
class AssignExpr;
class BinaryExpr;
class UnaryExpr;
class GroupingExpr;
class LiteralExpr;
class VariableExpr;

using Expression = std::unique_ptr<Expr>;

template <typename T, typename... Args>
std::unique_ptr<T> makeExpr(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

class ExprVisitor {
public:
	virtual LiteralValue visitAssignExpr(AssignExpr& expr) = 0;
	virtual LiteralValue visitBinaryExpr(BinaryExpr& expr) = 0;
	virtual LiteralValue visitUnaryExpr(UnaryExpr& expr) = 0;
	virtual LiteralValue visitGroupingExpr(GroupingExpr& expr) = 0;
	virtual LiteralValue visitLiteralExpr(LiteralExpr& expr) = 0;
	virtual LiteralValue visitVariableExpr(VariableExpr& expr) = 0;
	virtual ~ExprVisitor() = default;
};

class Expr {
public:
	virtual LiteralValue accept(ExprVisitor& visitor) = 0;
	virtual ~Expr() = default;
};

class AssignExpr : public Expr {
public:
	Token name;
	Expression value;

	AssignExpr(Token name, Expression value) : name(name), value(std::move(value)) {}
	LiteralValue accept(ExprVisitor& visitor) override {
		return visitor.visitAssignExpr(*this);
	}
};

class BinaryExpr : public Expr {
public:
	Expression left;
	Token Operator;
	Expression right;

	BinaryExpr(Expression left, Token Operator, Expression right) : left(std::move(left)), Operator(Operator), right(std::move(right)) {}
	LiteralValue accept(ExprVisitor& visitor) override {
		return visitor.visitBinaryExpr(*this);
	}
};

class UnaryExpr : public Expr {
public:
	Token Operator;
	Expression right;

	UnaryExpr(Token Operator, Expression right) : Operator(Operator), right(std::move(right)) {}
	LiteralValue accept(ExprVisitor& visitor) override {
		return visitor.visitUnaryExpr(*this);
	}
};

class GroupingExpr : public Expr {
public:
	Expression expression;

	GroupingExpr(Expression expression) : expression(std::move(expression)) {}
	LiteralValue accept(ExprVisitor& visitor) override {
		return visitor.visitGroupingExpr(*this);
	}
};

class LiteralExpr : public Expr {
public:
	LiteralValue literal;

	LiteralExpr(LiteralValue literal) : literal(literal) {}
	LiteralValue accept(ExprVisitor& visitor) override {
		return visitor.visitLiteralExpr(*this);
	}
};

class VariableExpr : public Expr {
public:
	Token name;

	VariableExpr(Token name) : name(name) {}
	LiteralValue accept(ExprVisitor& visitor) override {
		return visitor.visitVariableExpr(*this);
	}
};
