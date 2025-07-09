#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <iostream>
#include <limits>
#include "BigInt.hpp"

class BigDecimal;
class Integer {
    private:
        std::variant<int, int64_t, BigInt> value;

        Integer toInteger(const BigInt& num) const {
            if (num.fitsInInt()) {
                return Integer(num.toInt());
            } else if (num.fitsInInt64()) {
                return Integer(num.toInt64());
            } else {
                return Integer(num);
            }
        }

    public:
        Integer() : value(0) {}
        Integer(bool v) : value(static_cast<int>(v)) {}
        Integer(int v) : value(v) {}
        Integer(int64_t v) : value(v) {}
        Integer(const BigInt& v) : value(v) {}
        Integer(BigInt&& v) : value(std::move(v)) {}
        Integer(double v);
        Integer(BigDecimal v);

        Integer(const std::string& str) {
            try {
                value = std::stoi(str);
            } catch (const std::out_of_range&) {
                try {
                    value = std::stoll(str);
                } catch (const std::out_of_range&) {
                    value = BigInt(str);
                }
            }
        }

        std::string type() const {
            if (std::holds_alternative<int>(value)) return "int";
            if (std::holds_alternative<int64_t>(value)) return "int64";
            return "BigInt";
        }

        std::string toString() const {
            if (std::holds_alternative<int>(value))
                return std::to_string(std::get<int>(value));
            if (std::holds_alternative<int64_t>(value))
                return std::to_string(std::get<int64_t>(value));
            return std::get<BigInt>(value).toString();
        }

        bool isZero() const {
            return std::visit([](const auto& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, BigInt>) {
                    return arg.isZero();  // use your BigInt method
                } else {
                    return arg == 0;
                }
            }, value);
        }

        Integer abs() const {
            return std::visit([](const auto& arg) -> Integer {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, BigInt>) {
                    return arg.abs();
                } else {
                    return arg < 0 ? -arg : arg;
                }
            }, value);
        }

        friend std::ostream& operator<<(std::ostream& out, const Integer& num) {
            std::visit([&out](auto&& arg) {
                out << arg;
            }, num.value);
            return out;
        }

        Integer operator-() const {
            if (std::holds_alternative<int>(value))
                return Integer(-std::get<int>(value));
            if (std::holds_alternative<int64_t>(value))
                return Integer(-std::get<int64_t>(value));
            BigInt tmp = std::get<BigInt>(value);
            tmp.setSign(tmp.isPositive());
            return Integer(tmp);
        }

        bool operator==(const Integer& rhs) const {
            return value == rhs.value;
        }

        bool operator!=(const Integer& rhs) const {
            return value != rhs.value;
        }

        bool operator>(const Integer& rhs) const {
            return value > rhs.value;
        }

        bool operator<(const Integer& rhs) const {
            return value < rhs.value;
        }

        bool operator>=(const Integer& rhs) const {
            return value >= rhs.value;
        }

        bool operator<=(const Integer& rhs) const {
            return value <= rhs.value;
        }

        Integer add(BigInt lhs, BigInt rhs) const {
            BigInt res = lhs + rhs;
            return toInteger(res);
        }

        Integer add(BigInt lhs, int64_t rhs) const {
            BigInt res = lhs + BigInt(rhs);
            return toInteger(res);
        }

        Integer add(int64_t lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) + rhs;
            return toInteger(res);
        }

        Integer add(BigInt lhs, int rhs) const {
            BigInt res = lhs + BigInt(rhs);
            return toInteger(res);
        }

        Integer add(int lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) + rhs;
            return toInteger(res);
        }

        Integer add(int lhs, int rhs) const {
            int64_t result = static_cast<int64_t>(lhs) + static_cast<int64_t>(rhs);
            if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min())
                return Integer(static_cast<int64_t>(result));
            return Integer(static_cast<int>(result));
        }

        Integer add(int64_t lhs, int64_t rhs) const {
            BigInt res = BigInt(lhs) + BigInt(rhs);
            if (res.fitsInInt64())
                return Integer(res.toInt64());
            return Integer(res);
        }

        Integer add(int64_t lhs, int rhs) const {
            return add(lhs, static_cast<int64_t>(rhs));
        }

        Integer add(int lhs, int64_t rhs) const {
            return add(static_cast<int64_t>(lhs), rhs);
        }

        Integer operator+(const Integer& rhs) const {
            return std::visit([&](const auto& lhsVal) -> Integer {
                return std::visit([&](const auto& rhsVal) -> Integer {
                    return add(lhsVal, rhsVal);
                }, rhs.value);
            }, value);
        }

        Integer operator+=(const Integer& rhs) {
            *this = *this + rhs;
            return *this;
        }

        Integer& operator++(){
            *this += 1;
            return *this;
        }

        Integer operator++(int){
            Integer temp = *this;
            ++(*this);
            return temp;
        }

        Integer subtract(BigInt lhs, BigInt rhs) const {
            BigInt res = lhs - rhs;
            return toInteger(res);
        }

        Integer subtract(BigInt lhs, int64_t rhs) const {
            BigInt res = lhs - BigInt(rhs);
            return toInteger(res);
        }

        Integer subtract(int64_t lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) - rhs;
            return toInteger(res);
        }

        Integer subtract(BigInt lhs, int rhs) const {
            BigInt res = lhs - BigInt(rhs);
            return toInteger(res);
        }

        Integer subtract(int lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) - rhs;
            return toInteger(res);
        }

        Integer subtract(int lhs, int rhs) const {
            int64_t result = static_cast<int64_t>(lhs) - static_cast<int64_t>(rhs);
            if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min())
                return Integer(static_cast<int64_t>(result));
            return Integer(static_cast<int>(result));
        }

        Integer subtract(int64_t lhs, int64_t rhs) const {
            BigInt res = BigInt(lhs) - BigInt(rhs);
            if (res.fitsInInt64())
                return Integer(res.toInt64());
            return Integer(res);
        }

        Integer subtract(int64_t lhs, int rhs) const {
            return subtract(lhs, static_cast<int64_t>(rhs));
        }

        Integer subtract(int lhs, int64_t rhs) const {
            return subtract(static_cast<int64_t>(lhs), rhs);
        }

        Integer operator-(const Integer& rhs) const {
            return std::visit([&](const auto& lhsVal) -> Integer {
                return std::visit([&](const auto& rhsVal) -> Integer {
                    return subtract(lhsVal, rhsVal);
                }, rhs.value);
            }, value);
        }

        Integer operator-=(const Integer& rhs) {
            *this = *this - rhs;
            return *this;
        }

        Integer& operator--(){
            *this -= 1;
            return *this;
        }

        Integer operator--(int){
            Integer temp = *this;
            --(*this);
            return temp;
        }

        Integer multiply(BigInt lhs, BigInt rhs) const {
            BigInt res = lhs * rhs;
            return res;
        }

        Integer multiply(BigInt lhs, int64_t rhs) const {
            BigInt res = lhs * BigInt(rhs);
            return res;
        }

        Integer multiply(int64_t lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) * rhs;
            return res;
        }

        Integer multiply(BigInt lhs, int rhs) const {
            BigInt res = lhs * BigInt(rhs);
            return res;
        }

        Integer multiply(int lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) * rhs;
            return res;
        }

        Integer multiply(int lhs, int rhs) const {
            int64_t result = static_cast<int64_t>(lhs) * static_cast<int64_t>(rhs);
            if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min())
                return Integer(static_cast<int64_t>(result));
            return Integer(static_cast<int>(result));
        }

        Integer multiply(int64_t lhs, int64_t rhs) const {
            BigInt res = BigInt(lhs) * BigInt(rhs);
            if (res.fitsInInt64())
                return Integer(res.toInt64());
            return Integer(res);
        }

        Integer multiply(int64_t lhs, int rhs) const {
            return multiply(lhs, static_cast<int64_t>(rhs));
        }

        Integer multiply(int lhs, int64_t rhs) const {
            return multiply(static_cast<int64_t>(lhs), rhs);
        }

        Integer operator*(const Integer& rhs) const {
            return std::visit([&](const auto& lhsVal) -> Integer {
                return std::visit([&](const auto& rhsVal) -> Integer {
                    return multiply(lhsVal, rhsVal);
                }, rhs.value);
            }, value);
        }

        Integer operator*=(const Integer& rhs) {
            *this = *this * rhs;
            return *this;
        }

        Integer divide(BigInt lhs, BigInt rhs) const {
            BigInt res = lhs / rhs;
            return toInteger(res);
        }

        Integer divide(BigInt lhs, int64_t rhs) const {
            BigInt res = lhs / BigInt(rhs);
            return toInteger(res);
        }

        Integer divide(int64_t lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) / rhs;
            return toInteger(res);
        }

        Integer divide(BigInt lhs, int rhs) const {
            BigInt res = lhs / BigInt(rhs);
            return toInteger(res);
        }

        Integer divide(int lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) / rhs;
            return toInteger(res);
        }

        Integer divide(int lhs, int rhs) const {
            int64_t result = static_cast<int64_t>(lhs) / static_cast<int64_t>(rhs);
            if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min())
                return Integer(static_cast<int64_t>(result));
            return Integer(static_cast<int>(result));
        }

        Integer divide(int64_t lhs, int64_t rhs) const {
            BigInt res = BigInt(lhs) / BigInt(rhs);
            if (res.fitsInInt64())
                return Integer(res.toInt64());
            return Integer(res);
        }

        Integer divide(int64_t lhs, int rhs) const {
            return divide(lhs, static_cast<int64_t>(rhs));
        }

        Integer divide(int lhs, int64_t rhs) const {
            return divide(static_cast<int64_t>(lhs), rhs);
        }

        Integer operator/(const Integer& rhs) const {
            return std::visit([&](const auto& lhsVal) -> Integer {
                return std::visit([&](const auto& rhsVal) -> Integer {
                    if(rhsVal == 0)
                        throw std::runtime_error("Division by zero attempted");
                    return divide(lhsVal, rhsVal);
                }, rhs.value);
            }, value);
        }

        Integer operator/=(const Integer& rhs) {
            *this = *this / rhs;
            return *this;
        }

        Integer modulus(BigInt lhs, BigInt rhs) const {
            BigInt res = lhs % rhs;
            return toInteger(res);
        }

        Integer modulus(BigInt lhs, int64_t rhs) const {
            BigInt res = lhs % BigInt(rhs);
            return toInteger(res);
        }

        Integer modulus(int64_t lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) % rhs;
            return toInteger(res);
        }

        Integer modulus(BigInt lhs, int rhs) const {
            BigInt res = lhs % BigInt(rhs);
            return toInteger(res);
        }

        Integer modulus(int lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) % rhs;
            return toInteger(res);
        }

        Integer modulus(int lhs, int rhs) const {
            int64_t result = static_cast<int64_t>(lhs) % static_cast<int64_t>(rhs);
            if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min())
                return Integer(static_cast<int64_t>(result));
            return Integer(static_cast<int>(result));
        }

        Integer modulus(int64_t lhs, int64_t rhs) const {
            BigInt res = BigInt(lhs) % BigInt(rhs);
            if (res.fitsInInt64())
                return Integer(res.toInt64());
            return Integer(res);
        }

        Integer modulus(int64_t lhs, int rhs) const {
            return modulus(lhs, static_cast<int64_t>(rhs));
        }

        Integer modulus(int lhs, int64_t rhs) const {
            return modulus(static_cast<int64_t>(lhs), rhs);
        }

        Integer operator%(const Integer& rhs) const {
            return std::visit([&](const auto& lhsVal) -> Integer {
                return std::visit([&](const auto& rhsVal) -> Integer {
                    if(rhsVal == 0)
                        throw std::runtime_error("Division by zero attempted");
                    return modulus(lhsVal, rhsVal);
                }, rhs.value);
            }, value);
        }

        Integer operator%=(const Integer& rhs) {
            *this = *this % rhs;
            return *this;
        }

        double toDecimal() const {
             return std::visit([](const auto& arg) -> double {
                if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, BigInt>) {
                    return arg.toDecimal();
                } else {
                    return static_cast<double>(arg);
                }
            }, value);
        }

        bool toBool() const {
            if(isZero()) return false;
            return true;
        }

        BigDecimal toBigDecimal() const;
};
