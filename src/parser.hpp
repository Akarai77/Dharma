#pragma once
#include "Token.hpp"
#include "error.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "tokenType.hpp"
#include <algorithm>
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

        Token peek(){
            return tokens.at(current);
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
            if(check(type)) {
                return advance();
            }

            throw error(peek(),message);
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
                    case TokenType::VAR:
                    case TokenType::LET:
                    case TokenType::FOR:
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

                if (expectedType.has_value()) {
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
            while(match({TokenType::BANG,TokenType::MINUS})){
                Token Operator = previous();
                Expression right = getUnary();
                return makeExpr<UnaryExpr>(Operator,std::move(right));
            }
            
            return getPrimary(type);
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

        Expression getAssignment(std::optional<Token> type = std::nullopt){
            Expression expr = getEquality(type);

            if(match({TokenType::EQUAL})){
                Token equals = previous();
                Expression value = getAssignment();

                if(auto varExpr = dynamic_cast<VariableExpr*>(expr.get())){
                    Token name = varExpr->name;
                    return makeExpr<AssignExpr>(name,std::move(value));
                }

                Adharma::error(equals, "Invaid Assignment Target");
            }

            return expr;
        }

        Expression getExpression(std::optional<Token> type = std::nullopt){
            return getAssignment(type);            
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

        Statement getVarDeclaration(){
            Token name = consume(TokenType::IDENTIFIER,"Expect Variable name");
            
            std::optional<Token> type = std::nullopt;
            if(match({TokenType::COLON})) {
                if(peek().type == TokenType::TYPE) type = advance();
                else throw error(peek(),"Expect type after ':'");
            }

            Expression initializer = nullptr;
            if(match({TokenType::EQUAL})) {
                initializer = getExpression(type);
            }

            consume(TokenType::SEMICOLON,"Expect ';' after variable declaration.");
            return makeStmt<VarStmt>(name,type,std::move(initializer));
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
            if(match({TokenType::LEFT_BRACE})) return makeStmt<BlockStmt>(getBlockStmt());

            return getExprStmt();
        }

        Statement getDeclaration(){
            try{
                if(match({TokenType::VAR})) return getVarDeclaration();
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
