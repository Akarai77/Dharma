#pragma once

#include "types/conversions.hpp"
#include <optional>
#include <variant>
#include <string>

using LiteralCore = std::variant<Integer,double,BigDecimal,std::string,bool,Nil>;
using LiteralType = std::optional<LiteralCore>;
using LiteralValue = std::pair<LiteralCore,std::string>;

LiteralValue getLiteralData(const LiteralType& expr) {
    LiteralValue result;

    std::visit([&result](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        result.first = val;

        if constexpr (std::is_same_v<T, Integer>) {
            result.second = "integer";
        } else if constexpr (std::is_same_v<T, double>) {
            result.second = "decimal";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result.second = "string";
        } else if constexpr (std::is_same_v<T, BigDecimal>) {
            result.second = "BigDecimal";
        } else if constexpr (std::is_same_v<T, bool>) {
            result.second = "boolean";
        } else {
            result.second = "nil";
        }
    }, expr.value());

    return result;
}

bool isNil(const LiteralCore& val) {
    return std::holds_alternative<Nil>(val);
}

std::string cleanDouble(double val, int precision = 12) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << val;

    std::string str = oss.str();
    str.erase(str.find_last_not_of('0') + 1);
    if (str.back() == '.') str.pop_back();

    return str;
}

