#pragma once

#include "types/conversions.hpp"
#include <optional>
#include <variant>
#include <string>
#include <any>

using LiteralType = std::optional<std::variant<int, double, std::string, bool>>;
using LiteralValue = std::pair<std::any,std::string>;

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
            result.second = "bigDecimal";
        } else if constexpr (std::is_same_v<T, bool>) {
            result.second = "boolean";
        } else {
            result.second = "nil";
        }
    }, expr.value());

    return result;
}
