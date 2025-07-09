#pragma once
#include "token.hpp"
#include <unordered_map>
#include "error.hpp"
#include "util.hpp"


class Environment{
    private:
        Environment* enclosing;
        std::unordered_map<std::string, std::pair<RuntimeValue,std::string>> values;

    public:

        Environment() : enclosing(nullptr) {}

        Environment(Environment* enclosing) : enclosing(enclosing) {}

        void define(std::string name,RuntimeValue value,std::string type){
            values[name] = {value,type};
        }

        RuntimeValue get(Token name){
            if(values.contains(name.lexeme)){
                return values.at(name.lexeme).first;
            }

            if(enclosing != nullptr) return enclosing->get(name);

            throw RuntimeError(name, "Undefined Variable '" + name.lexeme +"'.");
        }

        void assign(Token name,LiteralValue value){
            if(values.contains(name.lexeme)){
                std::string type = values[name.lexeme].second;
                if(type == value.second || type == "var"){
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

        std::string toString(int depth = 0) const {
            std::string indent(depth * 2, ' ');
            std::string result = indent + "Environment (Depth " + std::to_string(depth) + "):\n";

            for (const auto& [name, valueTypePair] : values) {
                const auto& [value, type] = valueTypePair;
                std::string valStr;

                if (type == "integer") {
                    valStr = std::get<Integer>(getLiteralValue(value).first).toString();
                } else if (type == "decimal") {
                    valStr = cleanDouble(std::get<double>(getLiteralValue(value).first));
                } else if (type == "BigDecimal") {
                    valStr = std::get<BigDecimal>(getLiteralValue(value).first).toString();
                } else if (type == "boolean") {
                    valStr = std::get<bool>(getLiteralValue(value).first) ? "true" : "false";
                } else if (type == "string") {
                    valStr = "'" + std::get<std::string>((getLiteralValue(value).first)) + "'";
                } else if (type == "function") {
                    valStr = "<function>";
                } else if (type == "nil") {
                    valStr = "nil";
                } else {
                    valStr = "<unknown>";
                }

                result += indent + "  " + name + " : (" + type + ") = " + valStr + "\n";
            }

            if (enclosing) {
                result += indent + "-- Enclosing --\n";
                result += enclosing->toString(depth + 1);
            }

            return result;
        }

};

class EnvSwitch {
    Environment*& target;
    Environment* previous;

public:
    EnvSwitch(Environment*& target, Environment*& newEnv)
        : target(target), previous(target)
    {
        target = newEnv;
    }

    ~EnvSwitch() {
        target = previous;
    }
};

