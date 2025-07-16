#pragma once

#include "token.hpp"
#include <vector>
#include <memory>

class Expr;
class AssignExpr;
class BinaryExpr;
class CallExpr;
class UnaryExpr;
class GroupingExpr;
class GetExpr;
class SetExpr;
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
	virtual RuntimeValue visitAssignExpr(AssignExpr& expr) = 0;
	virtual RuntimeValue visitBinaryExpr(BinaryExpr& expr) = 0;
	virtual RuntimeValue visitCallExpr(CallExpr& expr) = 0;
	virtual RuntimeValue visitUnaryExpr(UnaryExpr& expr) = 0;
	virtual RuntimeValue visitGroupingExpr(GroupingExpr& expr) = 0;
	virtual RuntimeValue visitGetExpr(GetExpr& expr) = 0;
	virtual RuntimeValue visitSetExpr(SetExpr& expr) = 0;
	virtual RuntimeValue visitLiteralExpr(LiteralExpr& expr) = 0;
	virtual RuntimeValue visitLogicalExpr(LogicalExpr& expr) = 0;
	virtual RuntimeValue visitVariableExpr(VariableExpr& expr) = 0;
	virtual ~ExprVisitor() = default;
};

class Expr {
public:
	virtual RuntimeValue accept(ExprVisitor& visitor) = 0;
	virtual ~Expr() = default;
};

class AssignExpr : public Expr {
public:
	Token name;
	Token Operator;
	Expression value;

	AssignExpr(Token name, Token Operator, Expression value) : name(name), Operator(Operator), value(std::move(value)) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitAssignExpr(*this);
	}
};

class BinaryExpr : public Expr {
public:
	Expression left;
	Token Operator;
	Expression right;

	BinaryExpr(Expression left, Token Operator, Expression right) : left(std::move(left)), Operator(Operator), right(std::move(right)) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitBinaryExpr(*this);
	}
};

class CallExpr : public Expr {
public:
	Token name;
	Expression callee;
	Token paren;
	std::vector<Expression> arguments;

	CallExpr(Token name, Expression callee, Token paren, std::vector<Expression> arguments) : name(name), callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitCallExpr(*this);
	}
};

class UnaryExpr : public Expr {
public:
	Token Operator;
	Expression right;

	UnaryExpr(Token Operator, Expression right) : Operator(Operator), right(std::move(right)) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitUnaryExpr(*this);
	}
};

class GroupingExpr : public Expr {
public:
	Expression expression;

	GroupingExpr(Expression expression) : expression(std::move(expression)) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitGroupingExpr(*this);
	}
};

class GetExpr : public Expr {
public:
	Expression object;
	Token name;

	GetExpr(Expression object, Token name) : object(std::move(object)), name(name) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitGetExpr(*this);
	}
};

class SetExpr : public Expr {
public:
	Expression object;
	Token name;
	Expression value;

	SetExpr(Expression object, Token name, Expression value) : object(object), name(name), value(value) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitSetExpr(*this);
	}
};

class LiteralExpr : public Expr {
public:
	LiteralValue literal;

	LiteralExpr(LiteralValue literal) : literal(literal) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitLiteralExpr(*this);
	}
};

class LogicalExpr : public Expr {
public:
	Expression left;
	Token Operator;
	Expression right;

	LogicalExpr(Expression left, Token Operator, Expression right) : left(std::move(left)), Operator(Operator), right(std::move(right)) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitLogicalExpr(*this);
	}
};

class VariableExpr : public Expr {
public:
	Token name;

	VariableExpr(Token name) : name(name) {}
	RuntimeValue accept(ExprVisitor& visitor) override {
		return visitor.visitVariableExpr(*this);
	}
};

std::string getTypeOfExpression(const Expression& expr) {
	if (auto assignexpr = dynamic_cast<AssignExpr*>(expr.get())) {
		return "Assign Expression";
	} else if (auto binaryexpr = dynamic_cast<BinaryExpr*>(expr.get())) {
		return "Binary Expression";
	} else if (auto callexpr = dynamic_cast<CallExpr*>(expr.get())) {
		return "Call Expression";
	} else if (auto unaryexpr = dynamic_cast<UnaryExpr*>(expr.get())) {
		return "Unary Expression";
	} else if (auto groupingexpr = dynamic_cast<GroupingExpr*>(expr.get())) {
		return "Grouping Expression";
	} else if (auto getexpr = dynamic_cast<GetExpr*>(expr.get())) {
		return "Get Expression";
	} else if (auto setexpr = dynamic_cast<SetExpr*>(expr.get())) {
		return "Set Expression";
	} else if (auto literalexpr = dynamic_cast<LiteralExpr*>(expr.get())) {
		return "Literal Expression";
	} else if (auto logicalexpr = dynamic_cast<LogicalExpr*>(expr.get())) {
		return "Logical Expression";
	} else if (auto variableexpr = dynamic_cast<VariableExpr*>(expr.get())) {
		return "Variable Expression";
	}
	return "Unknown Expression";
}