#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>
#include <iostream>
#include <limits>
#include "custom/BigInt.hpp"

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
        Integer(int v) : value(v) {}
        Integer(int64_t v) : value(v) {}
        Integer(const BigInt& v) : value(v) {}
        Integer(BigInt&& v) : value(std::move(v)) {}

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

        Integer multiply(BigInt lhs, BigInt rhs) const {
            BigInt res = lhs * rhs;
            return toInteger(res);
        }

        Integer multiply(BigInt lhs, int64_t rhs) const {
            BigInt res = lhs * BigInt(rhs);
            return toInteger(res);
        }

        Integer multiply(int64_t lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) * rhs;
            return toInteger(res);
        }

        Integer multiply(BigInt lhs, int rhs) const {
            BigInt res = lhs * BigInt(rhs);
            return toInteger(res);
        }

        Integer multiply(int lhs, BigInt rhs) const {
            BigInt res = BigInt(lhs) * rhs;
            return toInteger(res);
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
};


int main() {
    Integer a(INT32_MAX);                         //  2147483647
    Integer b(INT64_MAX);                         //  9223372036854775807
    Integer c("9223372036854775808");             // > int64_t
    Integer d(INT32_MIN);                         // -2147483648
    Integer e(INT64_MIN);                         // -9223372036854775808
    Integer f("-9223372036854775809");            // < int64_t

    // Subtraction tests (existing)
    std::cout << "a - 1: " << (a - Integer(1)).toString() << " (" << (a - Integer(1)).type() << ")\n";
    std::cout << "a - (-1): " << (a - Integer(-1)).toString() << " (" << (a - Integer(-1)).type() << ")\n";
    std::cout << "b - 1: " << (b - Integer(1)).toString() << " (" << (b - Integer(1)).type() << ")\n";
    std::cout << "b - (-1): " << (b - Integer(-1)).toString() << " (" << (b - Integer(-1)).type() << ")\n";
    std::cout << "c - 1: " << (c - Integer(1)).toString() << " (" << (c - Integer(1)).type() << ")\n";
    std::cout << "d - 1: " << (d - Integer(1)).toString() << " (" << (d - Integer(1)).type() << ")\n";
    std::cout << "d - (-1): " << (d - Integer(-1)).toString() << " (" << (d - Integer(-1)).type() << ")\n";
    std::cout << "e - 1: " << (e - Integer(1)).toString() << " (" << (e - Integer(1)).type() << ")\n";
    std::cout << "e - (-1): " << (e - Integer(-1)).toString() << " (" << (e - Integer(-1)).type() << ")\n";
    std::cout << "f - 1: " << (f - Integer(1)).toString() << " (" << (f - Integer(1)).type() << ")\n";
    std::cout << "f - (-1): " << (f - Integer(-1)).toString() << " (" << (f - Integer(-1)).type() << ")\n";

    std::cout << "-a: " << (-a).toString() << " (" << (-a).type() << ")\n";
    std::cout << "-b: " << (-b).toString() << " (" << (-b).type() << ")\n";
    std::cout << "-c: " << (-c).toString() << " (" << (-c).type() << ")\n";

    // *** Multiplication tests ***
    std::cout << "\n-- Multiplication tests --\n";

    std::cout << "a * 2: " << (a * Integer(2)).toString() << " (" << (a * Integer(2)).type() << ")\n";
    std::cout << "a * (-2): " << (a * Integer(-2)).toString() << " (" << (a * Integer(-2)).type() << ")\n";

    std::cout << "b * 2: " << (b * Integer(2)).toString() << " (" << (b * Integer(2)).type() << ")\n";
    std::cout << "b * (-2): " << (b * Integer(-2)).toString() << " (" << (b * Integer(-2)).type() << ")\n";

    std::cout << "c * 2: " << (c * Integer(2)).toString() << " (" << (c * Integer(2)).type() << ")\n";

    std::cout << "d * 2: " << (d * Integer(2)).toString() << " (" << (d * Integer(2)).type() << ")\n";
    std::cout << "d * (-2): " << (d * Integer(-2)).toString() << " (" << (d * Integer(-2)).type() << ")\n";

    std::cout << "e * 2: " << (e * Integer(2)).toString() << " (" << (e * Integer(2)).type() << ")\n";
    std::cout << "e * (-2): " << (e * Integer(-2)).toString() << " (" << (e * Integer(-2)).type() << ")\n";

    std::cout << "f * 2: " << (f * Integer(2)).toString() << " (" << (f * Integer(2)).type() << ")\n";
    std::cout << "f * (-2): " << (f * Integer(-2)).toString() << " (" << (f * Integer(-2)).type() << ")\n";

    return 0;
}

