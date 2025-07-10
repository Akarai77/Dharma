/*
program         -> declaration* EOF ;
declaration     -> funcDecl
                |  varDecl
                |  statement ;
statement       -> exprStmt
                |  ifStmt
                |  whileStmt
                |  forStmt
                |  printStmt
                |  returnStmt
                |  block ;
funcDecl        -> "fun" IDENTIFIER "(" (IDENTIFIER ("," IDENTIFIER)*)? ")" block;
varDecl         -> ("var"|"int"|"boolean"|"decimal"|"BigDecimal"|"string") IDENTIFIER (":" (int"|"decimal"|"BigDecimal"|string"|"boolean"))? ( "=" expression )? ";";
exprStmt        -> expression ";" ;
ifStmt          -> "if" "(" expression ")" statement
                (  "else" statement )? ;
whileStmt       -> "while" "( expression )" statement ;
forStmt         -> "for" "(" varDecl | exprStmt | ";" )
                    expresion?    ";"
                    expression? ")" statement ;
printStmt       -> "print" expression ";" ;
returnStmt      -> "return" expression? ";" ;
block           -> "{" declaration* "}" ;
expression      -> assignment ;
assignment      -> IDENTIFIER ("=" | "+=" | "-=" | "*=" | "/=" | "%=") assignment
                |  logic_or ;
logic_or        -> logic_and (("or" | "||") logic_and)*;
logic_and       -> equality (("and" | "&&") equality)*;
equality        -> comparison ( ( "!=" | "==" ) comparison )* ;
comparison      -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term            -> factor ( ( "-" | "+" ) factor )* ;
factor          -> unary ( ( "/" | "*" ) unary )* ;
unary           -> ( "!" | "-" | "++" | "--" ) unary
                | call ;
call            -> primary ("++"|"--")?  ( "(" arguments? ")")* ;
arguments       -> expression ("," expression) * ;
primary         -> VARIABLE | "true" | "false" | "nil"
                |  "(" expression ")" ;
*/

#pragma once
#include "util.hpp"
#include "token.hpp"
#include "error.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "tokenType.hpp"
#include "warning.hpp"
#include <algorithm>
#include <bits/chrono.h>
#include <initializer_list>
#include <optional>
#include <unordered_map>
#include <vector>

class Parser{
    private:
        std::vector<Token> tokens;
        int current = 0;

        bool isAtEnd(){
            if(tokens.at(current).type == TokenType::EOF_TOKEN) return true;
            else return false;
        }

        Token peek(int offset = 0){
            return tokens.at(current + offset);
        }

        Token& previous(int offset = 1){
            return tokens.at(current-offset);
        }

        Token advance(){
            if(!isAtEnd()) current++;
            return previous();
        }

        bool check(TokenType type){
            if(isAtEnd()) return false;
            if(peek().type == type) return true;
            else return false;
        }

        bool match(std::initializer_list<TokenType> types){
            for(TokenType type : types){
                if(check(type)){
                    advance();
                    return true;
                }
            }
            return false;
        }

        Token consume(TokenType type, std::string message){
            if(type == TokenType::SEMICOLON) {
                if(check(type)) return advance();
                else {
                    Token t = Token(TokenType::SEMICOLON,";",std::nullopt,previous().line,previous().column+previous().lexeme.size());
                    if(!SEMICOLON_WARNING_SUPPRESS)
                        std::cout<<SemiColonWarning(t).message();
                    return t;
                }
            }
            if(check(type)) {
                return advance();
            }

            throw ParseError(peek(),message);
        }

        Token mapCompoundToBinary(Token Operator) {
            switch (Operator.type) {
                case TokenType::PLUS_EQUAL: return Token(TokenType::PLUS, "+", std::nullopt, Operator.line,Operator.column);
                case TokenType::MINUS_EQUAL: return Token(TokenType::MINUS, "-", std::nullopt, Operator.line,Operator.column);
                case TokenType::STAR_EQUAL: return Token(TokenType::STAR, "*", std::nullopt, Operator.line,Operator.column);
                case TokenType::SLASH_EQUAL: return Token(TokenType::SLASH, "/", std::nullopt, Operator.line,Operator.column);
                case TokenType::PERCENT_EQUAL: return Token(TokenType::PERCENT, "%",std::nullopt,Operator.line,Operator.column);
                default: throw ParseError(Operator,"Unknown Compound operator : '"+Operator.lexeme+"'.");
            }
        }


        bool isConvertible(std::string from, std::string to) {
            static std::unordered_map<std::string, std::vector<std::string>> conversionTable = {
                {"decimal", {"integer","BigDecimal"}},
                {"BigDecimal", {"integer","decimal"}},
                {"integer", {"decimal","BigDecimal","boolean"}},
                {"boolean", {"integer"}},
                {"nil",{"integer","decimal","boolean","BigDecimal"}}
            };

            auto it = conversionTable.find(from);
            if (it != conversionTable.end()) {
                return std::find(it->second.begin(), it->second.end(), to) != it->second.end();
            }

            return false;
        }

        LiteralCore performConversion(LiteralCore value, const std::string& from, const std::string& to) {
            if(from == "decimal" && to == "integer") return Integer(std::get<double>(value));
            if(from == "decimal" && to == "BigDecimal") return BigDecimal(std::get<double>(value));
            if(from == "BigDecimal" && to == "integer") return Integer(std::get<BigDecimal>(value));
            if(from == "BigDecimal" && to == "decimal") return (std::get<BigDecimal>(value)).toDecimal();
            if(from == "integer" && to == "decimal") return (std::get<Integer>(value)).toDecimal();
            if(from == "integer" && to == "BigDecimal") return BigDecimal(std::get<Integer>(value));
            if(from == "integer" && to == "boolean") return (std::get<Integer>(value)).toBool();
            if(from == "boolean" && to == "integer") return Integer(std::get<bool>(value));
            if(from == "nil"){
                if(to == "integer") return Integer(0);
                if(to == "decimal") return static_cast<double>(0);
                if(to == "BigDecimal") return BigDecimal(0);
                if(to == "boolean") return false;
            }
            return Nil();
        }

        void synchronize(){
            advance();

            while(!isAtEnd()){
                if(peek().type == TokenType::SEMICOLON){
                    advance(); 
                    return;
                }

                switch(peek().type){
                    case TokenType::CLASS:
                    case TokenType::FUN:
                    case TokenType::FOR:
                    case TokenType::TYPE:
                    case TokenType::IF:
                    case TokenType::WHILE:
                    case TokenType::PRINT:
                    case TokenType::RETURN:
                        return;
                }

                advance();
            }
        }
       
        Expression getPrimary(std::optional<Token> expectedType = std::nullopt){
            if(match({TokenType::TRUE})) {
                std::string actualType = "boolean";

                if (expectedType.has_value()) {
                    std::string expected = expectedType->lexeme;

                    if (actualType != expected) {
                        if (!isConvertible(actualType, expected)) {
                            throw ParseError(previous(), "Type mismatch: cannot convert 'boolean' to '" + expected + "'");
                        }

                        LiteralCore convertedValue = performConversion(true, actualType, expected);
                        return makeExpr<LiteralExpr>(LiteralValue{convertedValue, expected});
                    }
                }

                return makeExpr<LiteralExpr>(LiteralValue{true, actualType});
            }

            if(match({TokenType::FALSE})) {
                std::string actualType = "boolean";

                if (expectedType.has_value()) {
                    std::string expected = expectedType->lexeme;

                    if (actualType != expected) {
                        if (!isConvertible(actualType, expected)) {
                            throw ParseError(previous(), "Type mismatch: cannot convert 'boolean' to '" + expected + "'");
                        }

                        LiteralCore convertedValue = performConversion(false, actualType, expected);
                        return makeExpr<LiteralExpr>(LiteralValue{convertedValue, expected});
                    }
                }

                return makeExpr<LiteralExpr>(LiteralValue{false, actualType});
            }

            if(match({TokenType::NIL})) {
                std::string actualType = "nil";

                if (expectedType.has_value()) {
                    std::string expected = expectedType->lexeme;

                    if (actualType != expected) {
                        if (!isConvertible(actualType, expected)) {
                            throw ParseError(previous(), "Type mismatch: cannot convert 'nil' to '" + expected + "'");
                        }

                        LiteralCore convertedValue = performConversion(Nil(), actualType, expected);
                        return makeExpr<LiteralExpr>(LiteralValue{convertedValue, expected});
                    }
                }

                return makeExpr<LiteralExpr>(LiteralValue{Nil(), actualType});
            }

            if(match({TokenType::VARIABLE})) {
                LiteralValue lit = getLiteralData(previous().literal);

                std::string actual = lit.second;
                if (expectedType.has_value()) {
                    std::string expected = expectedType->lexeme;
                    if(expected == "int") expected = "integer";
                    if(expected == "integer") previous().type = TokenType::INTEGER;
                    if(expected == "decimal")  previous().type = TokenType::DECIMAL;
                    if(expected == "BigDecimal") previous().type = TokenType::BIGDECIMAL;
                    if(expected == "string") previous().type = TokenType::STRING;
                    if(expected == "boolean") previous().type = TokenType::BOOLEAN;

                    if (actual != expected) {
                        if (!isConvertible(actual, expected)) {
                            throw ParseError(previous(), "Type mismatch: cannot convert '" + actual + "' to '" + expected + "'");
                        }

                        LiteralCore convertedValue = performConversion(lit.first, actual, expected);
                        return makeExpr<LiteralExpr>(LiteralValue{convertedValue, expected});
                    }
                }
                
                if(actual == "BigDecimal") {
                    BigDecimal val = std::get<BigDecimal>(lit.first);
                    if(val.fitsInDecimal()){
                        lit = {val.toDecimal(),"decimal"};
                    }
                }
                return makeExpr<LiteralExpr>(lit);
            }

            if(match({TokenType::IDENTIFIER})) {
                return makeExpr<VariableExpr>(previous());
            }

            if(match({TokenType::LEFT_PAREN})){
                Expression expr = getExpression(expectedType);
                consume(TokenType::RIGHT_PAREN,"Expected ')' after expression.");
                return makeExpr<GroupingExpr>(std::move(expr));
            }

            throw ParseError(peek(),"Expect Expression");
        }

        Expression getCall(std::optional<Token> type = std::nullopt) {
            Expression expr = getPrimary(type);

            if (match({TokenType::PLUS_PLUS,TokenType::MINUS_MINUS})) {
                Token Operator = previous();
                Operator.type = (Operator.type == TokenType::PLUS_PLUS) ? TokenType::POST_INCR : TokenType::POST_DECR;
                return makeExpr<UnaryExpr>(Operator, std::move(expr));
            }

            while(true) {
                if(match({TokenType::LEFT_PAREN})) {
                    std::vector<Expression> arguments;
                    if(!check(TokenType::RIGHT_PAREN)) {
                        do {
                            if(arguments.size() > 255)
                                std::cerr<<ParseError(peek(),"Cant have more than 255 arguments!").message();

                            arguments.push_back(getExpression());
                        } while(match({TokenType::COMMA}));
                    }
                    Token paren = consume(TokenType::RIGHT_PAREN,"Expect ')' after arguments.");
                    auto varExpr = dynamic_cast<VariableExpr*>(expr.get());
                    expr = makeExpr<CallExpr>(varExpr->name,std::move(expr),paren,std::move(arguments));
                } else {
                    break;
                }
            }

            return expr;
        }

        Expression getUnary(std::optional<Token> type = std::nullopt){
            if(match({TokenType::BANG,TokenType::MINUS,TokenType::PLUS_PLUS,TokenType::MINUS_MINUS})){
                Token Operator = previous();
                if(Operator.type == TokenType::PLUS_PLUS)
                    Operator.type = TokenType::PRE_INCR;
                else if(Operator.type == TokenType::MINUS_MINUS)
                    Operator.type = TokenType::PRE_DECR;
                Expression right = getUnary();
                return makeExpr<UnaryExpr>(Operator,std::move(right));
            }
            
            return getCall(type);
        }

        Expression getFactor(std::optional<Token> type = std::nullopt){
            Expression expr = getUnary(type);

            while(match({TokenType::STAR,TokenType::SLASH,TokenType::PERCENT})){
                Token Operator = previous();
                Expression right = getUnary();
                expr = makeExpr<BinaryExpr>(std::move(expr),Operator,std::move(right));
            }
            
            return expr;
        }

        Expression getTerm(std::optional<Token> type = std::nullopt){
            Expression expr = getFactor(type);

            while(match({TokenType::PLUS,TokenType::MINUS})){
                Token Operator = previous();
                Expression right = getFactor();
                expr = makeExpr<BinaryExpr>(std::move(expr),Operator,std::move(right));
            }
            
            return expr;
        }

        Expression getComparison(std::optional<Token> type = std::nullopt){
            Expression expr = getTerm(type);

            while(match({TokenType::GREATER,TokenType::GREATER_EQUAL,TokenType::LESS,TokenType::LESS_EQUAL})){
                Token Operator = previous();
                Expression right = getTerm();
                expr = makeExpr<BinaryExpr>(std::move(expr),Operator,std::move(right));
            }

            return expr;
        }

        Expression getEquality(std::optional<Token> type = std::nullopt){
            Expression expr = getComparison(type);
        
            while(match({TokenType::BANG_EQUAL,TokenType::EQUAL_EQUAL})){
                Token Operator = previous();
                Expression right = getComparison();
                expr = makeExpr<BinaryExpr>(std::move(expr),Operator,std::move(right));
            }

            return expr;
        }

        Expression getLogicalAnd(std::optional<Token> type = std::nullopt){
            Expression expr = getEquality(type);

            while(match({TokenType::AND,TokenType::AMP_AMP})){
                Token Operator = previous();
                Expression right = getEquality();
                expr = makeExpr<LogicalExpr>(std::move(expr),Operator,std::move(right));
            }

            return expr;
        }

        Expression getLogicalOr(std::optional<Token> type = std::nullopt){
            Expression expr = getLogicalAnd(type);

            while(match({TokenType::OR,TokenType::PIPE_PIPE})){
                Token Operator = previous();
                Expression right = getLogicalAnd();
                expr = makeExpr<LogicalExpr>(std::move(expr),Operator,std::move(right));
            }

            return expr;
        }

        Expression getAssignment(std::optional<Token> type = std::nullopt){
            Expression expr = getLogicalOr(type);

            if(match({TokenType::EQUAL,TokenType::PLUS_EQUAL,TokenType::MINUS_EQUAL,
                        TokenType::STAR_EQUAL,TokenType::SLASH_EQUAL,TokenType::PERCENT_EQUAL})){
                Token Operator = previous();
                Expression value = getAssignment();

                if(auto varExpr = dynamic_cast<VariableExpr*>(expr.get())){
                    if(Operator.type != TokenType::EQUAL){
                        value = makeExpr<BinaryExpr>(
                                std::move(expr),
                                mapCompoundToBinary(Operator),
                                std::move(value)
                               );
                    }

                    Token name = varExpr->name;
                    return makeExpr<AssignExpr>(name,Operator,std::move(value));
                }

                throw ParseError(Operator, "Assignment Target cant be a '"+getTypeOfExpression(std::move(expr))+"'.");
            }

            return expr;
        }

        Expression getExpression(std::optional<Token> type = std::nullopt){
            return getAssignment(type);            
        }

        Statement getFunctionStatement(std::string kind) {
            Token name = consume(TokenType::IDENTIFIER,"Expect" + kind + "name.");
            consume(TokenType::LEFT_PAREN,"Expect '(' after "+kind+" name.");
            std::vector<Statement> parameters;
            if(!check(TokenType::RIGHT_PAREN)){
                do {
                    if(parameters.size() > 255) {
                        throw ParseError(peek(),"Cannot have more than 255 parameters.");
                    }
                    
                    if(match({TokenType::TYPE})) {
                        parameters.push_back(
                            getVarDeclaration(previous(),false)
                        );
                    } else {
                        throw ParseError(peek(),"Expect Variable Declaration.");
                    }
                } while (match({TokenType::COMMA}));
            }

            consume(TokenType::RIGHT_PAREN,"Expect ')' after parameters.");
            Token retType = Token(TokenType::TYPE,"var",std::nullopt,previous().line,previous().column);
            if(match({TokenType::ARROW})) {
                retType = consume(TokenType::TYPE,"Expect return type after '->'.");
            }
            consume(TokenType::LEFT_BRACE,"Expect '{' before "+kind+" body.");

            std::vector<Statement> body = std::move(getBlockStatement(retType));
            return makeStmt<FunctionStmt>(name,kind,std::move(parameters),std::move(body),retType);
        }

        Statement getVarDeclaration(Token type,bool semiColon = true){
            Token name = consume(TokenType::IDENTIFIER,"Expect Variable name");

            if(match({TokenType::COLON})){
                Token annotatedType = consume(TokenType::TYPE,"Expected Type after ':'");
                if(annotatedType.lexeme == "var")
                    throw ParseError(previous(),"Invalid type annotation: 'var' cannot be used as a type annotation.");
                if(type.lexeme != "var"){
                    if(type.lexeme == annotatedType.lexeme)
                        throw ParseError(previous(),"Redundant type annotation: variable '"+
                                name.lexeme+"' already declared as '"+type.lexeme+"'.");
                    else
                        throw ParseError(previous(),"Conflicting type annotations for '"+
                                name.lexeme+"' : declared as '"+type.lexeme+
                                "' but annotated as '"+annotatedType.lexeme+"'.");
                }
                type = annotatedType;
            }
            
            Expression initializer = nullptr;
            if(match({TokenType::EQUAL})) {
                initializer = getExpression(std::make_optional<Token>(type));
            }
    
            if(semiColon)
                consume(TokenType::SEMICOLON,"Expect ';' after variable declaration.");
            return makeStmt<VarStmt>(name,type,std::move(initializer));
        }

        Statement getPrintStatement(){
            Expression value = getExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after expression.");
            return makeStmt<PrintStmt>(std::move(value));
        }

        Statement getExprStatement(){
            Expression expression = getExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after expression.");
            return makeStmt<ExprStmt>(std::move(expression));
        }

        Statement getIfStatement(){
            consume(TokenType::LEFT_PAREN,"Expect '(' after if");
            Expression ifCondition = getExpression();
            consume(TokenType::RIGHT_PAREN,"Expect ')' after condition");

            Statement thenBranch = getStatement();
            Expression elifCondition = nullptr;
            Statement elifBranch = nullptr;
            if(match({TokenType::ELIF})){
                consume(TokenType::LEFT_PAREN,"Expect '(' after elif");
                elifCondition = getExpression();
                consume(TokenType::RIGHT_PAREN,"Expect ')' after condition");
                elifBranch = getStatement();
            }
            
            Statement elseBranch = nullptr;
            if(match({TokenType::ELSE})){
                elseBranch = getStatement();
            }

            return makeStmt<IfStmt>(
                    std::move(ifCondition),
                    std::move(thenBranch),
                    std::move(elifCondition),
                    std::move(elifBranch),
                    std::move(elseBranch)
                  );
        }

        Statement getWhileStatement(){
            consume(TokenType::LEFT_PAREN,"Expect '(' after while.");
            Expression condition = getExpression();
            consume(TokenType::RIGHT_PAREN,"Expect ')' after condition.");
            
            Statement body = getStatement();

            return makeStmt<WhileStmt>(std::move(condition),std::move(body));
        }

        Statement getForStatement(){
            consume(TokenType::LEFT_PAREN,"Expect '(' after for.");

            Statement initializer;
            if(match({TokenType::SEMICOLON})) {
                initializer = nullptr;
            } else if(match({TokenType::TYPE})) {
                initializer = getVarDeclaration(previous());
            } else {
                initializer = getExprStatement();
            }

            Expression condition = nullptr;
            if(!check(TokenType::SEMICOLON)){
                condition = getExpression();
            }
            consume(TokenType::SEMICOLON,"Expect ';' after loop condition.");

            Expression increment = nullptr;
            if(!check(TokenType::RIGHT_PAREN)){
                increment = getExpression();
            }
            
            consume(TokenType::RIGHT_PAREN,"Expect ')' after clauses.");
            Statement body = getStatement();

            return makeStmt<ForStmt>(
                    std::move(initializer),
                    std::move(condition),
                    std::move(increment),
                    std::move(body)
                   );

        }

        Statement getReturnStatement(std::optional<Token> retType = std::nullopt) {
            Token keyword = previous();
            Expression value = nullptr;
            if(!check(TokenType::SEMICOLON)) {
                value = getExpression();
            }
            consume(TokenType::SEMICOLON,"Expect ';' after return value.");
            return makeStmt<ReturnStmt>(keyword,std::move(value),retType.value());
        }

        std::vector<Statement> getBlockStatement(std::optional<Token> retType = std::nullopt){
            std::vector<Statement> statements;

            while(!isAtEnd() && !check(TokenType::RIGHT_BRACE)) {
                statements.push_back(getDeclaration(retType));
            }

            consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
            return statements;
        }

        Statement getStatement(std::optional<Token> retType = std::nullopt){
            if(match({TokenType::FUN})) return getFunctionStatement("function");
            if(match({TokenType::PRINT})) return getPrintStatement();
            if(match({TokenType::IF})) return getIfStatement();
            if(match({TokenType::WHILE})) return getWhileStatement();
            if(match({TokenType::FOR})) return getForStatement();
            if(match({TokenType::RETURN})) return getReturnStatement(retType);
            if(match({TokenType::LEFT_BRACE})) return makeStmt<BlockStmt>(getBlockStatement());

            return getExprStatement();
        }

        Statement getDeclaration(std::optional<Token> retType = std::nullopt){
            try{
                if(match({TokenType::TYPE})) return getVarDeclaration(previous());
                return getStatement(retType);
           } catch(ParseError& error){
                std::cerr<<error.message();
                synchronize();
                return nullptr;
            }
        }

    public:
        Parser(std::vector<Token> tokens) : tokens(tokens) {}

        std::vector<Statement> parse() {
            std::vector<Statement> statements;
            while (!isAtEnd()) {
                Statement stmt = getDeclaration();
                if (stmt != nullptr) {
                    statements.push_back(std::move(stmt));
                }
            }
            return statements;
        }
};
