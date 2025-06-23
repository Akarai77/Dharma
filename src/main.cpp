#include <cstdio>
#include <cstdlib>
#include <fstream>
#include<iostream>
#include <sstream>
#include <string>
#include <vector>
#include "error.hpp"
#include "parser.hpp"
#include "stmt.hpp"
#include "tokenizer.hpp"
#include "interpreter.hpp"

void run(const std::string& script){
    Tokenizer tokenizer(script);
    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    std::vector<Statement> statements = parser.parse();
    Interpreter interpreter;

    if(errorFlag) return;
    if(runtimeErrorFlag) return;
    interpreter.interpret(statements);
}

void runFile(const std::string& path){
    std::ifstream input;
    input.open(path);
    std::stringstream scriptStream;
    scriptStream << input.rdbuf();
    const std::string script = scriptStream.str();
    run(script);
    if(errorFlag == true) exit(EXIT_FAILURE);
}

void runPrompt(){
    std::string input;
    while(true){
        std::cout<<"> ";
        std::getline(std::cin, input);
        if(input == "") break;
        run(input);
        errorFlag = false;
        runtimeErrorFlag = false;
        std::cout<<std::endl;
    }
    std::cout<<"Thank You! May Your soul find the right path!";
}

int main(int argc, char** argv){
   if(argc > 2){
        std::cerr<<"Usage karma [script]\n";
        exit(EXIT_FAILURE);
   } else if(argc == 2) {
        runFile(argv[1]);
   } else {
        runPrompt();
   }
    return EXIT_SUCCESS;
}
