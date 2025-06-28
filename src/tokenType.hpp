#pragma once
enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, SEMICOLON, COLON,
    MINUS, PLUS, SLASH, STAR, PERCENT,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    ARROW,                  
    FAT_ARROW,             
    PLUS_EQUAL, MINUS_EQUAL,
    STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL,
    PLUS_PLUS, POST_INCR, PRE_INCR,
    MINUS_MINUS, POST_DECR, PRE_DECR,
    QUESTION, QUESTION_COLON,
    PIPE, PIPE_PIPE,
    AMP, AMP_AMP,

    // Literals
    IDENTIFIER,
    VARIABLE,
    STRING,
    INT,
    DECIMAL,
    BOOLEAN,
    NIL,    
    TYPE,  

    // Keywords
    AND, OR, NOT,
    IF, ELIF, ELSE,
    FOR, WHILE, LOOP,
    BREAK, CONTINUE, CONST,
    FUN, RETURN,
    MATCH, CASE, DEFAULT,
    CLASS, SUPER, THIS, EXTENDS, NEW,
    TRUE, FALSE,
    IMPORT, FROM, AS,
    USE, MODULE,
    EXPORT,
    ASYNC, AWAIT,

    // 🧘 Special Dharma Primitives
    CHANT,               // print
    MEDITATE,            // sleep / pause
    INVOKE,              // interpret another file
    YIELD,               // for coroutines / generators
    SUTRA,               // possibly to define internal script blocks

    // Meta / structural tokens
    COMMENT,
    WHITESPACE,
    NEWLINE,

    // End of file
    EOF_TOKEN
};
