#pragma once

#include "Token.hpp"
#include <memory>

class Expr;
class AssignExpr;
class BinaryExpr;
class UnaryExpr;
class GroupingExpr;
class LiteralExpr;
class LogicalExpr;
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
	virtual LiteralValue visitLogicalExpr(LogicalExpr& expr) = 0;
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
	Token Operator;
	Expression value;

	AssignExpr(Token name, Token Operator, Expression value) : name(name), Operator(Operator), value(std::move(value)) {}
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

class LogicalExpr : public Expr {
public:
	Expression left;
	Token Operator;
	Expression right;

	LogicalExpr(Expression left, Token Operator, Expression right) : left(std::move(left)), Operator(Operator), right(std::move(right)) {}
	LiteralValue accept(ExprVisitor& visitor) override {
		return visitor.visitLogicalExpr(*this);
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

std::string getTypeOfExpression(Expression expr) {
	if (auto assignexpr = dynamic_cast<AssignExpr*>(expr.get())) {
		return "Assign Expression";
	} else if (auto binaryexpr = dynamic_cast<BinaryExpr*>(expr.get())) {
		return "Binary Expression";
	} else if (auto unaryexpr = dynamic_cast<UnaryExpr*>(expr.get())) {
		return "Unary Expression";
	} else if (auto groupingexpr = dynamic_cast<GroupingExpr*>(expr.get())) {
		return "Grouping Expression";
	} else if (auto literalexpr = dynamic_cast<LiteralExpr*>(expr.get())) {
		return "Literal Expression";
	} else if (auto logicalexpr = dynamic_cast<LogicalExpr*>(expr.get())) {
		return "Logical Expression";
	} else if (auto variableexpr = dynamic_cast<VariableExpr*>(expr.get())) {
		return "Variable Expression";
	}
	return "Unknown Expression";
}
