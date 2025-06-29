/*
program        -> declaration* EOF ;
declaration    -> varDecl
               |  statement ;
statement      -> exprStmt
               |  ifStmt
               |  printStmt
               |  block ;
ifStmt         -> "if" "(" expression ")" statement
               (  "else" statement )? ;
block          -> "{" declaration* "}" ;
exprStmt       -> expression ";" ;
printStmt      -> "print" expression ";" ;
varDecl        -> ("var"|"int"|"boolean"|"decimal"|"string") IDENTIFIER (":" (int"|"decimal"|"string"|"boolean"))? ( "=" expression )? ";";
expression     -> assignment ;
assignment     -> IDENTIFIER ("=" | "+=" | "-=" | "*=" | "/=" | "%=") assignment
               |  logic_or ;
logic_or       -> logic_and (("or" | "||") logic_and)*;
logic_and      -> equality (("and" | "&&") equality)*;
equality       -> comparison ( ( "!=" | "==" ) comparison )* ;
comparison     -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           -> factor ( ( "-" | "+" ) factor )* ;
factor         -> unary ( ( "/" | "*" ) unary )* ;
unary          -> ( "!" | "-" | "++" | "--" ) unary
               |  primary ("++"|"--")? ;
primary        -> VARIABLE | "true" | "false" | "nil"
               |  "(" expression ")" ;
*/

#pragma once
#include "Token.hpp"
#include "error.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "tokenType.hpp"
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
                else return Token(TokenType::SEMICOLON,";",std::nullopt,peek().line); //warn implicit ;
            }
            if(check(type)) {
                return advance();
            }

            throw error(peek(),message);
        }

        Token mapCompoundToBinary(Token Operator) {
            switch (Operator.type) {
                case TokenType::PLUS_EQUAL: return Token(TokenType::PLUS, "+", std::nullopt, Operator.line);
                case TokenType::MINUS_EQUAL: return Token(TokenType::MINUS, "-", std::nullopt, Operator.line);
                case TokenType::STAR_EQUAL: return Token(TokenType::STAR, "*", std::nullopt, Operator.line);
                case TokenType::SLASH_EQUAL: return Token(TokenType::SLASH, "/", std::nullopt, Operator.line);
                case TokenType::PERCENT_EQUAL: return Token(TokenType::PERCENT, "%",std::nullopt,Operator.line);
                default: throw std::runtime_error("Unknown compound operator");
            }
        }


        bool isConvertible(std::string from, std::string to) {
            static std::unordered_map<std::string, std::vector<std::string>> conversionTable = {
                {"int", {"decimal"}},
                {"decimal", {"int"}},
                {"boolean", {"int"}},
                {"int", {"boolean"}},
                {"nil",{"int","decimal","boolean"}}
            };

            auto it = conversionTable.find(from);
            if (it != conversionTable.end()) {
                return std::find(it->second.begin(), it->second.end(), to) != it->second.end();
            }

            return false;
        }

        std::any performConversion(std::any value, const std::string& from, const std::string& to) {
            if(from == "decimal" && to == "int") return static_cast<int>(std::any_cast<double>(value));
            if(from == "int" && to == "decimal") return static_cast<double>(std::any_cast<int>(value));
            if(from == "int" && to == "boolean") return static_cast<bool>(std::any_cast<int>(value));
            if(from == "boolean" && to == "int") return static_cast<int>(std::any_cast<bool>(value));
            if(from == "nil"){
                if(to == "int" || to == "decimal") return 0;
                if(to == "boolean") return false;
            }
            throw std::runtime_error("Unsupported implicit conversion.");
        }

        ParseError error(Token token,std::string message){
            Adharma::error(token,message);
            ParseError parseError;
            return parseError;
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
                    case TokenType::CHANT:
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
                            throw error(previous(), "Type mismatch: cannot convert 'boolean' to '" + expected + "'");
                        }

                        std::any convertedValue = performConversion(true, actualType, expected);
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
                            throw error(previous(), "Type mismatch: cannot convert 'boolean' to '" + expected + "'");
                        }

                        std::any convertedValue = performConversion(false, actualType, expected);
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
                            throw error(previous(), "Type mismatch: cannot convert 'nil' to '" + expected + "'");
                        }

                        std::any convertedValue = performConversion("nil", actualType, expected);
                        return makeExpr<LiteralExpr>(LiteralValue{convertedValue, expected});
                    }
                }

                return makeExpr<LiteralExpr>(LiteralValue{"nil", actualType});
            }

            if(match({TokenType::VARIABLE})) {
                LiteralValue lit = getLiteralData(previous().literal);

                if (expectedType.has_value() && expectedType.value().lexeme != "var") {
                    if(expectedType.value().lexeme == "int") previous().type = TokenType::INT;
                    if(expectedType.value().lexeme == "decimal") previous().type = TokenType::DECIMAL;
                    if(expectedType.value().lexeme == "string") previous().type = TokenType::STRING;
                    if(expectedType.value().lexeme == "boolean") previous().type = TokenType::BOOLEAN;
                    std::string expected = expectedType->lexeme;
                    std::string actual = lit.second;

                    if (actual != expected) {
                        if (!isConvertible(actual, expected)) {
                            throw error(previous(), "Type mismatch: cannot convert '" + actual + "' to '" + expected + "'");
                        }

                        std::any convertedValue = performConversion(lit.first, actual, expected);
                        return makeExpr<LiteralExpr>(LiteralValue{convertedValue, expected});
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

            throw error(peek(),"Expect Expression");
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
            
            Expression expr = getPrimary(type);

            if (match({TokenType::PLUS_PLUS,TokenType::MINUS_MINUS})) {
                Token Operator = previous();
                Operator.type = (Operator.type == TokenType::PLUS_PLUS) ? TokenType::POST_INCR : TokenType::POST_DECR;
                return makeExpr<UnaryExpr>(Operator, std::move(expr));
            }

            return expr;
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

            if(match({TokenType::EQUAL,TokenType::PLUS_EQUAL,TokenType::MINUS_EQUAL,TokenType::STAR_EQUAL,TokenType::SLASH_EQUAL,TokenType::PERCENT_EQUAL})){
                Token Operator = previous();
                Expression value = getAssignment();

                if(auto varExpr = dynamic_cast<VariableExpr*>(expr.get())){
                    if(Operator.type != TokenType::EQUAL){
                        value = makeExpr<BinaryExpr>(std::move(expr),mapCompoundToBinary(Operator),std::move(value));
                    }

                    Token name = varExpr->name;
                    return makeExpr<AssignExpr>(name,Operator,std::move(value));
                }

                Adharma::error(Operator, "Invaid Assignment Target");
            }

            return expr;
        }

        Expression getExpression(std::optional<Token> type = std::nullopt){
            return getAssignment(type);            
        }

        Statement getVarDeclaration(Token type){
            Token name = consume(TokenType::IDENTIFIER,"Expect Variable name");

            if(match({TokenType::COLON})){
                Token annotatedType = consume(TokenType::TYPE,"Expected Type after ':'");
                if(annotatedType.lexeme == "var")
                    throw error(previous(),"Invalid type annotation: 'var' cannot be used as a type annotation.");
                if(type.lexeme != "var"){
                    if(type.lexeme == annotatedType.lexeme)
                        throw error(previous(),"Redundant type annotation: variable '"+name.lexeme+"' already declared as '"+type.lexeme+"'.");
                    else
                        throw error(previous(),"Conflicting type annotations for '"+name.lexeme+"' : declared as '"+type.lexeme+"' but annotated as '"+annotatedType.lexeme+"'.");
                }
                type = annotatedType;
            }
            
            Expression initializer = nullptr;
            if(match({TokenType::EQUAL})) {
                initializer = getExpression(std::make_optional<Token>(type));
            }

            consume(TokenType::SEMICOLON,"Expect ';' after variable declaration.");
            return makeStmt<VarStmt>(name,type,std::move(initializer));
        }

        Statement getPrintStmt(){
            Expression value = getExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after expression.");
            return makeStmt<PrintStmt>(std::move(value));
        }

        Statement getExprStmt(){
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

            return makeStmt<IfStmt>(std::move(ifCondition),std::move(thenBranch),std::move(elifCondition),std::move(elifBranch),std::move(elseBranch));
        }

        std::vector<Statement> getBlockStmt(){
            std::vector<Statement> statements;

            while(!isAtEnd() && !check(TokenType::RIGHT_BRACE)) {
                statements.push_back(getDeclaration());
            }

            consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
            return statements;
        }

        Statement getStatement(){
            if(match({TokenType::CHANT})) return getPrintStmt();
            if(match({TokenType::IF})) return getIfStatement();
            if(match({TokenType::LEFT_BRACE})) return makeStmt<BlockStmt>(getBlockStmt());

            return getExprStmt();
        }

        Statement getDeclaration(){
            try{
                if(match({TokenType::TYPE})) return getVarDeclaration(previous());
                return getStatement();
            } catch(ParseError& error){
                synchronize();
                return nullptr;
            }
        }

    public:
        Parser(std::vector<Token> tokens) : tokens(tokens) {}

        std::vector<Statement> parse() {
            std::vector<Statement> statements;
            while(!isAtEnd())
                statements.push_back(getDeclaration());
            return statements;
        }
};
