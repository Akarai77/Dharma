#pragma once

#include "expr.hpp"
#include <memory>

class Stmt;
class BlockStmt;
class ClassStmt;
class ExprStmt;
class PrintStmt;
class VarStmt;
class FunctionStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class ReturnStmt;

using Statement = std::unique_ptr<Stmt>;

template <typename T, typename... Args>
std::unique_ptr<T> makeStmt(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

class StmtVisitor {
public:
	virtual RuntimeValue visitBlockStmt(BlockStmt& stmt) = 0;
	virtual RuntimeValue visitClassStmt(ClassStmt& stmt) = 0;
	virtual RuntimeValue visitExprStmt(ExprStmt& stmt) = 0;
	virtual RuntimeValue visitPrintStmt(PrintStmt& stmt) = 0;
	virtual RuntimeValue visitVarStmt(VarStmt& stmt) = 0;
	virtual RuntimeValue visitFunctionStmt(FunctionStmt& stmt) = 0;
	virtual RuntimeValue visitIfStmt(IfStmt& stmt) = 0;
	virtual RuntimeValue visitWhileStmt(WhileStmt& stmt) = 0;
	virtual RuntimeValue visitForStmt(ForStmt& stmt) = 0;
	virtual RuntimeValue visitReturnStmt(ReturnStmt& stmt) = 0;
	virtual ~StmtVisitor() = default;
};

class Stmt {
public:
	virtual RuntimeValue accept(StmtVisitor& visitor) = 0;
	virtual ~Stmt() = default;
};

class BlockStmt : public Stmt {
public:
	std::vector<Statement> statements;

	BlockStmt(std::vector<Statement>&& statements) : statements(std::move(statements)) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitBlockStmt(*this);
	}
};

class ClassStmt : public Stmt {
public:
	Token name;
	Expression superclass;
	std::vector<FunctionStmt> methods;

	ClassStmt(Token name, Expression superclass, std::vector<FunctionStmt>&& methods) : name(name), superclass(std::move(superclass)), methods(std::move(methods)) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitClassStmt(*this);
	}
};

class ExprStmt : public Stmt {
public:
	Expression expression;

	ExprStmt(Expression expression) : expression(std::move(expression)) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitExprStmt(*this);
	}
};

class PrintStmt : public Stmt {
public:
	Expression expression;

	PrintStmt(Expression expression) : expression(std::move(expression)) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitPrintStmt(*this);
	}
};

class VarStmt : public Stmt {
public:
	Token name;
	Token type;
	Expression initializer;

	VarStmt(Token name, Token type, Expression initializer) : name(name), type(type), initializer(std::move(initializer)) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitVarStmt(*this);
	}
};

class FunctionStmt : public Stmt {
public:
	Token name;
	std::string kind;
	std::vector<Statement> params;
	std::vector<Statement> body;
	Token returnType;

	FunctionStmt(Token name, std::string kind, std::vector<Statement>&& params, std::vector<Statement>&& body, Token returnType) : name(name), kind(kind), params(std::move(params)), body(std::move(body)), returnType(returnType) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitFunctionStmt(*this);
	}
};

class IfStmt : public Stmt {
public:
	Expression ifCondition;
	Statement thenBranch;
	Expression elifCondition;
	Statement elifBranch;
	Statement elseBranch;

	IfStmt(Expression ifCondition, Statement thenBranch, Expression elifCondition, Statement elifBranch, Statement elseBranch) : ifCondition(std::move(ifCondition)), thenBranch(std::move(thenBranch)), elifCondition(std::move(elifCondition)), elifBranch(std::move(elifBranch)), elseBranch(std::move(elseBranch)) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitIfStmt(*this);
	}
};

class WhileStmt : public Stmt {
public:
	Expression condition;
	Statement body;

	WhileStmt(Expression condition, Statement body) : condition(std::move(condition)), body(std::move(body)) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitWhileStmt(*this);
	}
};

class ForStmt : public Stmt {
public:
	Statement initializer;
	Expression condition;
	Expression increment;
	Statement body;

	ForStmt(Statement initializer, Expression condition, Expression increment, Statement body) : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitForStmt(*this);
	}
};

class ReturnStmt : public Stmt {
public:
	Token keyword;
	Expression value;
	Token retType;

	ReturnStmt(Token keyword, Expression value, Token retType) : keyword(keyword), value(std::move(value)), retType(retType) {}
	RuntimeValue accept(StmtVisitor& visitor) override {
		return visitor.visitReturnStmt(*this);
	}
};

std::string getTypeOfExpression(const Statement& stmt) {
	if (auto blockstmt = dynamic_cast<BlockStmt*>(stmt.get())) {
		return "BlockStmt Expression";
	} else if (auto classstmt = dynamic_cast<ClassStmt*>(stmt.get())) {
		return "ClassStmt Expression";
	} else if (auto exprstmt = dynamic_cast<ExprStmt*>(stmt.get())) {
		return "Stmt Expression";
	} else if (auto printstmt = dynamic_cast<PrintStmt*>(stmt.get())) {
		return "PrintStmt Expression";
	} else if (auto varstmt = dynamic_cast<VarStmt*>(stmt.get())) {
		return "VarStmt Expression";
	} else if (auto functionstmt = dynamic_cast<FunctionStmt*>(stmt.get())) {
		return "FunctionStmt Expression";
	} else if (auto ifstmt = dynamic_cast<IfStmt*>(stmt.get())) {
		return "IfStmt Expression";
	} else if (auto whilestmt = dynamic_cast<WhileStmt*>(stmt.get())) {
		return "WhileStmt Expression";
	} else if (auto forstmt = dynamic_cast<ForStmt*>(stmt.get())) {
		return "ForStmt Expression";
	} else if (auto returnstmt = dynamic_cast<ReturnStmt*>(stmt.get())) {
		return "ReturnStmt Expression";
	}
	return "Unknown Expression";
}