#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
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
        interpreter.interpret(statements);
}

void runFile(const std::string& path){
    std::ifstream input;
    input.open(path);
    std::stringstream scriptStream;
    scriptStream << input.rdbuf();
    const std::string script = scriptStream.str();
    try {
        run(script);
    } catch(const Adharma& err) {
        std::cerr<<err.message()<<std::endl;
    }
}

void runPrompt(){
    std::string input;
    while(true){
        std::cout<<"> ";
        std::getline(std::cin, input);
        if(input == "") break;
        try {
            run(input);
        } catch (const Adharma& err) {
            std::cerr<<err.message()<<std::endl;
        }
        std::cout<<std::endl;
    }
    std::cout<<"Thank You! May Your soul find the right path!";
}

int main(int argc, char** argv){
   if(argc > 2){
        std::cerr<<"Usage karma [script]\n";
        exit(EXIT_FAILURE);
   } else if(argc == 2) {
       std::filesystem::path filePath = argv[1];
        if(filePath.extension() != ".dh"){
            std::cerr<<"Invalid file input to sutra!";
            exit(EXIT_FAILURE);
        }
        if(!std::filesystem::exists(argv[1])){
            std::cerr<<"File "<<argv[1]<<" does not exist!";
            exit(EXIT_FAILURE);
        }
        runFile(argv[1]);
   } else {
        runPrompt();
   }
    return EXIT_SUCCESS;
}
