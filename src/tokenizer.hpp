#pragma once
#include "ASTPrinter.hpp"
#include"Token.hpp"
#include"error.hpp"
#include <cctype>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Tokenizer{
    private:
        std::string source;
        std::vector<Token> tokens;
        int start = 0;
        int current = 0;
        int line = 1;
        int column = 1;
        bool semicolonFlag = false;
        TokenType lastSignificant = TokenType::EOF_TOKEN;

        std::unordered_map<std::string, TokenType> keywords = {
            {"and"        ,TokenType::AND},
            {"or"         ,TokenType::OR},
            {"not"        ,TokenType::NOT},
            {"if"         ,TokenType::IF},
            {"elif"       ,TokenType::ELIF},
            {"else"       ,TokenType::ELSE},
            {"for"        ,TokenType::FOR},
            {"while"      ,TokenType::WHILE},
            {"loop"       ,TokenType::LOOP},
            {"break"      ,TokenType::BREAK},
            {"continue"   ,TokenType::CONTINUE},
            {"print"      ,TokenType::CHANT},
            {"const"      ,TokenType::CONST},
            {"fun"        ,TokenType::FUN},
            {"return"     ,TokenType::RETURN},
            {"match"      ,TokenType::MATCH},
            {"case"       ,TokenType::CASE},
            {"default"    ,TokenType::DEFAULT},
            {"class"      ,TokenType::CLASS},
            {"super"      ,TokenType::SUPER},
            {"this"       ,TokenType::THIS},
            {"extends"    ,TokenType::EXTENDS},
            {"new"        ,TokenType::NEW},
            {"true"       ,TokenType::TRUE},
            {"false"      ,TokenType::FALSE},
            {"nil"        ,TokenType::NIL},
            {"import"     ,TokenType::IMPORT},
            {"from"       ,TokenType::FROM},
            {"as"         ,TokenType::AS},
            {"use"        ,TokenType::USE},
            {"module"     ,TokenType::MODULE},
            {"export"     ,TokenType::EXPORT},
            {"async"      ,TokenType::ASYNC},
            {"await"      ,TokenType::AWAIT},
            {"var"        ,TokenType::TYPE},
            {"int"        ,TokenType::TYPE},
            {"decimal"    ,TokenType::TYPE},
            {"BigDecimal" ,TokenType::TYPE},
            {"string"     ,TokenType::TYPE},
            {"boolean"    ,TokenType::TYPE}
        };

        bool isAtEnd(){
            return current >= source.length();
        }

        char peek(int offset = 0) {
            return ( isAtEnd() || current + offset >= source.length() ) ? '\0' : source.at(current + offset);
        }

        char advance(){
            return source.at(current++);
        }

        void addToken(TokenType type,LiteralType literal = std::nullopt){
            std::string lexeme;
            if(type == TokenType::SEMICOLON) {
                lexeme = ";";
            } else {
                lexeme = source.substr(start,current-start);
            }
            lastSignificant = type;
            tokens.push_back(Token(type,lexeme,literal,line));
        }

        bool match(char expected){
            if(isAtEnd() || source.at(current) != expected) return false;
            current++;
            return true;
        }

        void getString(char quote){
            while(!isAtEnd() && peek() != quote){
                if(peek() == '\n') line++;
                advance();
            }

            if(isAtEnd()){
                Adharma::error(peek(), "Unterminated String");
                return;
            }

            advance();

            std::string str = source.substr(start+1,current-start-2);
            addToken(TokenType::VARIABLE,str);
        }

        void getNumber(){
            while(std::isdigit(peek())) advance();

            LiteralType number;
            if(peek() == '.' && std::isdigit(peek(1))){
                advance();
                while(std::isdigit(peek())) advance();
                addToken(TokenType::VARIABLE,std::stod(source.substr(start,current-start)));
                return;
            }

           addToken(TokenType::VARIABLE,std::stoi(source.substr(start,current-start)));           
        }

        bool isAlpha(char ch){
            return std::isalpha(ch) || ch == '_';
        }

        bool isAlnum(char ch){
            return isAlpha(ch) || std::isdigit(ch);
        }
        
        void getIdentifier(){
            while(isAlnum(peek())) advance();
           
            std::string text = source.substr(start,current-start);
            auto it = keywords.find(text);
            TokenType type;
            if (it == keywords.end()) 
                type = TokenType::IDENTIFIER;
            else
                type = it->second;
            addToken(type);
        }

        bool canInsertSemicolonAfter(TokenType type) {
            return type != TokenType::SEMICOLON && (type == TokenType::IDENTIFIER ||
                type == TokenType::VARIABLE ||
                type == TokenType::TRUE ||
                type == TokenType::FALSE ||
                type == TokenType::TYPE ||
                type == TokenType::NIL);
        }

        void scanToken(){
            char c = advance();
            switch(c){
                case '(' : addToken(TokenType::LEFT_PAREN); break;
                case ')' : addToken(TokenType::RIGHT_PAREN); break;
                case '{' : addToken(TokenType::LEFT_BRACE); break;
                case '}' : addToken(TokenType::RIGHT_BRACE); break;
                case '[' : addToken(TokenType::LEFT_BRACKET); break;
                case ']' : addToken(TokenType::RIGHT_BRACKET); break;
                case ',' : addToken(TokenType::COMMA); break;
                case '.' : addToken(TokenType::DOT); break;
                case ';' : addToken(TokenType::SEMICOLON); semicolonFlag = true; break;
                case ':' : addToken(TokenType::COLON); break;
                case '-':
                           if (match('='))       addToken(TokenType::MINUS_EQUAL);
                           else if (match('>'))  addToken(TokenType::ARROW);
                           else if (match('-'))  addToken(TokenType::MINUS_MINUS);
                           else                  addToken(TokenType::MINUS);
                           break;
                case '+':
                           if (match('='))       addToken(TokenType::PLUS_EQUAL);
                           else if (match('+'))  addToken(TokenType::PLUS_PLUS);
                           else                  addToken(TokenType::PLUS);
                           break;
                case '*' : addToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR); break;
                case '%' : addToken(match('=') ? TokenType::PERCENT_EQUAL : TokenType::PERCENT); break;
                case '!' : addToken(match('=') ? TokenType::BANG_EQUAL :  (match('>') ? TokenType::FAT_ARROW : TokenType::EQUAL)); break;
                case '=' : addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
                case '<' : addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
                case '>' : addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
                case '/' : if(match('/')){
                               while(peek() != '\n' && !isAtEnd()) advance();
                           } else if(match('=')){
                                addToken(TokenType::SLASH_EQUAL);
                           } else {
                               addToken(TokenType::SLASH);
                           }
                           break;
                case '?' : addToken(match(':') ? TokenType::QUESTION_COLON : TokenType::QUESTION); break;
                case '|' : addToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE); break;
                case '&' : addToken(match('&') ? TokenType::AMP_AMP : TokenType::AMP); break;
                case ' ' : 
                case '\r' :
                case '\t' : break;
                case '\n' : line++; break;
                
                case '\'' : getString('\''); break;
                case '"'  : getString('"'); break;
                
                default: if(std::isdigit(c)){
                             getNumber();
                         } else if(isAlpha(c)) {
                             getIdentifier();
                         } else {
                             Adharma::error(line, "Unexpected Character");
                         }
                         break;

            }
        }

    public:
        Tokenizer(std::string source): source(source) {}

        std::vector<Token> tokenize(){
            
            while(!isAtEnd()){
                start = current;
                scanToken();
            }


            tokens.push_back(Token(TokenType::EOF_TOKEN,"",std::nullopt,line));
            return tokens;
        }

};
