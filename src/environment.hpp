#include "Token.hpp"
#include <memory>
#include <unordered_map>
#include "error.hpp"

class Environment{
    private:
        std::unique_ptr<Environment> enclosing;
        std::unordered_map<std::string, std::pair<LiteralValue,std::string>> values;

    public:

        Environment() : enclosing(nullptr) {}

        Environment(Environment* enclosing) : enclosing(enclosing) {}

        void define(std::string name,LiteralValue value,std::string type){
            values.insert({name,{value,type}});
        }

        LiteralValue get(Token name){
            if(values.contains(name.lexeme)){
                return values.at(name.lexeme).first;
            }

            if(enclosing != nullptr) return enclosing->get(name);

            throw RuntimeError(name, "Undefined Variable '" + name.lexeme +"'.");
        }

        void assign(Token name,LiteralValue value){
            if(values.contains(name.lexeme)){
                std::string type = values[name.lexeme].second;
                if(type == value.second || type == "variable"){
                    values[name.lexeme].first = value;
                    return;
                }
                throw RuntimeError(name,"Cannot convert '"+value.second+"' to '"+type+"'");
            }

            if(enclosing != nullptr){ 
                enclosing->assign(name,value);
                return;
            }

            throw RuntimeError(name,"Undefined variable '"+name.lexeme+"'.");
        }
};

class EnvGuard {
    Environment*& target;
    Environment* previous;

    public:
        EnvGuard(Environment*& target) : target(target), previous(target) {}

        ~EnvGuard() {
            target = previous;
        }
};
