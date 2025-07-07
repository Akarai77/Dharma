#include "BigInt.hpp"
#include "BigDecimal.hpp"
#include "Integer.hpp"
#include "Nil.hpp"

BigDecimal BigInt::sqrt(size_t precision) const {
    if (isNegative)
        throw std::invalid_argument("Cannot compute square root of negative BigInt");

    BigDecimal dec(*this);
    return dec.sqrt(precision);
}

BigInt BigInt::operator=(const BigDecimal& rhs) {
    *this = rhs.truncate();
    return *this;
}

BigDecimal intToDecimal(BigInt integer, size_t decimalPointPos) {
    std::string num = integer.toString();
    bool isNeg = !integer.isPositive();
    if (isNeg) num = num.substr(1);

    if (decimalPointPos >= num.length()) {
        num.insert(0, std::string(decimalPointPos - num.length() + 1, '0'));
    }

    num.insert(num.length() - decimalPointPos, ".");

    if (isNeg)
        num.insert(0, "-");

    return BigDecimal(num);
}

Integer::Integer(double v) {
    if (v >= static_cast<double>(INT32_MIN) && v <= static_cast<double>(INT32_MAX)) {
        value = static_cast<int>(v);
    } else if (v >= static_cast<double>(INT64_MIN) && v <= static_cast<double>(INT64_MAX)) {
        value = static_cast<int64_t>(v);
    } else {
        BigDecimal bd(v);
        value = bd.truncate();
    }
}

Integer::Integer(BigDecimal v) : value(v.truncate()) {}


BigDecimal Integer::toBigDecimal() const {
    return std::visit([](auto& arg) -> BigDecimal {
        return BigDecimal(arg);
    }, value);
}

BigDecimal::BigDecimal(Integer num) {
    *this = num.toBigDecimal();
}
