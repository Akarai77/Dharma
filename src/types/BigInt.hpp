#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class BigDecimal;
class BigInt {
    private:
        std::vector<uint8_t> digits;
        bool isNegative = false;

    public:
        BigInt() : digits(1, 0), isNegative(false) {}
        BigInt(const std::string& str) { parseFromString(str); }
        BigInt(uint8_t digit) { digits.push_back(digit); }
        BigInt(size_t digit) {digits.push_back(digit); }
        BigInt(int num) { *this = BigInt(std::to_string(num)); }
        BigInt(int64_t num) { *this = BigInt(std::to_string(num)); }

        void removeLeadingZeros() {
            while (digits.size() > 1 && digits.back() == 0) {
                digits.pop_back();
            }

            if (digits.size() == 1 && digits[0] == 0) {
                isNegative = false;
            }
        }

        void parseFromString(const std::string& str) {
            digits.clear();
            isNegative = false;

            size_t start = 0;
            if(str[0]=='+'){
                start = 1;
            }
            if (str[0] == '-') {
                isNegative = true;
                start = 1;
            }

            for (size_t i = str.size(); i > start; i--) {
                char c = str[i - 1];
                if (!std::isdigit(c)) throw std::invalid_argument("Invalid digit in BigInt string.");
                digits.push_back(c - '0');
            }

            removeLeadingZeros();
        }

        bool isZero() const {
            return digits.size() == 1 && digits[0] == 0;
        }

        bool isPositive() const {
            return !isNegative;
        }

        int length() const {
            return digits.size();
        }

        void flipSign() {
            if (!isZero()) isNegative = !isNegative;
        }

        void setSign(bool sign) {
            isNegative = sign;
        }

        friend std::ostream& operator<<(std::ostream& out, const BigInt& integer) {
            if (integer.isNegative)
                out << "-";

            for (auto it = integer.digits.rbegin(); it != integer.digits.rend(); it++)
                out << static_cast<int>(*it);

            return out;
        }

        std::string toString() const {
            std::string res = isNegative ? "-" : "";
            for (auto it = digits.rbegin(); it != digits.rend(); it++) {
                res.push_back(*it + '0');
            }
            return res;
        }

        BigInt operator-() const {
            BigInt temp = *this;
            temp.flipSign();
            return temp;
        }

        bool operator>(const BigInt& rhs) const {
            if (isNegative != rhs.isNegative)
                return !isNegative;

            bool result = false;
            if (digits.size() != rhs.digits.size()) {
                result = digits.size() > rhs.digits.size();
            } else {
                for (size_t i = digits.size(); i-- > 0;) {
                    if (digits[i] != rhs.digits[i]) {
                        result = digits[i] > rhs.digits[i];
                        break;
                    }
                }
            }

            return isNegative ? !result : result;
        }

        bool operator<(const BigInt& rhs) const {
            return !(*this >= rhs);
        }

        bool operator==(const BigInt& rhs) const {
            return (isNegative == rhs.isNegative) && (digits == rhs.digits);
        }

        bool operator>=(const BigInt& rhs) const {
            return (*this > rhs) || (*this == rhs);
        }

        bool operator<=(const BigInt& rhs) const {
            return (*this < rhs) || (*this == rhs);
        }

        bool operator!=(const BigInt& rhs) const {
            return !(*this == rhs);
        }

        BigInt operator+(const BigInt& rhs) const {
            if (isZero()) return rhs;
            if (rhs.isZero()) return *this;

            BigInt result;
            result.digits.clear();
            if (isNegative == rhs.isNegative) {
                result.isNegative = isNegative;
                size_t maxlen = std::max(digits.size(), rhs.digits.size());
                uint8_t carry = 0;

                for (size_t i = 0; i < maxlen || carry; ++i) {
                    uint8_t lhsDigit = (i < digits.size()) ? digits[i] : 0;
                    uint8_t rhsDigit = (i < rhs.digits.size()) ? rhs.digits[i] : 0;
                    uint8_t sum = lhsDigit + rhsDigit + carry;
                    carry = sum / 10;
                    result.digits.push_back(sum % 10);
                }

                result.removeLeadingZeros();
                return result;
            }

            result = *this - (-rhs);
            result.isNegative = !isNegative;
            return result;
        }

        BigInt operator+=(const BigInt& rhs) {
            *this = *this + rhs;
            return *this;
        }

        BigInt& operator++() {
            *this += BigInt(1);
            return *this;
        }

        BigInt operator++(int) {
            BigInt temp = *this;
            ++(*this);
            return temp;
        }

        BigInt operator-(const BigInt& rhs) const {
            if (isZero()) return -rhs;
            if (rhs.isZero()) return *this;

            if (isNegative != rhs.isNegative) return *this + (-rhs);

            BigInt a = *this;
            BigInt b = rhs;
            a.isNegative = b.isNegative = false;

            if (a < b) {
                BigInt result = b - a;
                result.isNegative = !isNegative;
                return result;
            } else {
                BigInt result;
                result.digits.clear();
                uint8_t borrow = 0;

                for (size_t i = 0; i < a.digits.size(); ++i) {
                    int lhsDigit = a.digits[i] - borrow;
                    int rhsDigit = (i < b.digits.size()) ? b.digits[i] : 0;

                    if (lhsDigit < rhsDigit) {
                        lhsDigit += 10;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }

                    result.digits.push_back(lhsDigit - rhsDigit);
                }

                result.removeLeadingZeros();
                result.isNegative = isNegative;
                return result;
            }
        }

        BigInt operator-=(const BigInt& rhs) {
            *this = *this - rhs;
            return *this;
        }

        BigInt& operator--() {
            *this -= BigInt(1);
            return *this;
        }

        BigInt operator--(int) {
            BigInt temp = *this;
            --(*this);
            return temp;
        }

        std::pair<BigInt, BigInt> splitAt(size_t index) const {
            BigInt high, low;
            low.digits.assign(digits.begin(), digits.begin() + std::min(index, digits.size()));
            if (index < digits.size())
                high.digits.assign(digits.begin() + index, digits.end());
            high.removeLeadingZeros();
            low.removeLeadingZeros();
            return {high, low};
        }

        BigInt shiftLeft(size_t index) const {
            if(isZero()) return BigInt("0");
            BigInt res = *this;
            res.digits.insert(res.digits.begin(),index,0);
            return res;
        }

        BigInt naiveMultiply(const BigInt& rhs) const {
            BigInt result;
            result.digits = std::vector<uint8_t>(digits.size() + rhs.digits.size(), 0);

            for (size_t i = 0; i < digits.size(); ++i) {
                int carry = 0;
                for (size_t j = 0; j < rhs.digits.size(); ++j) {
                    int prod = digits[i] * rhs.digits[j] + result.digits[i + j] + carry;
                    result.digits[i + j] = prod % 10;
                    carry = prod / 10;
                }
                if (carry > 0) result.digits[i + rhs.digits.size()] += carry;
            }

            result.removeLeadingZeros();
            return result;
        }

        BigInt karatsubaMultiply(const BigInt& rhs) const {
            size_t n = std::max(digits.size(),rhs.digits.size());
            if(n<=32) return naiveMultiply(rhs);

            size_t m = n/2;

            auto [a1,a0] = this->splitAt(m);
            auto [b1,b0] = rhs.splitAt(m);

            BigInt z0 = a0.karatsubaMultiply(b0);
            BigInt z2 = a1.karatsubaMultiply(b1);
            BigInt z1 = (a0+a1).karatsubaMultiply(b0+b1) - z2 - z0;

            BigInt result = (z2.shiftLeft(2*m)) + (z1.shiftLeft(m)) + z0;
            return result;
        }

        BigInt operator*(const BigInt& rhs) const {
            if(isZero() || rhs.isZero()) return BigInt("0");

            BigInt result = karatsubaMultiply(rhs);
            result.isNegative = (isNegative != rhs.isNegative);
            result.removeLeadingZeros();
            return result;
        }

        BigInt operator*=(const BigInt& rhs){
            *this = *this * rhs;
            return *this;
        }

        std::pair<BigInt, BigInt> divmod(const BigInt& rhs) const {
            BigInt quotient, remainder;
            quotient.digits.resize(digits.size(), 0);
            for (int i = digits.size() - 1; i >= 0; i--) {
                remainder.digits.insert(remainder.digits.begin(), digits[i]);
                remainder.removeLeadingZeros();

                uint8_t low = 0, high = 9, qtDigit = 0;
                while (low <= high) {
                    uint8_t mid = (low + high) / 2;
                    BigInt trial = rhs * BigInt(mid);
                    if (trial <= remainder) {
                        qtDigit = mid;
                        low = mid + 1;
                    } else {
                        high = mid - 1;
                    }
                }

                quotient.digits[i] = qtDigit;
                remainder -= rhs * BigInt(qtDigit);
            }

            quotient.removeLeadingZeros();
            remainder.removeLeadingZeros();
            return {quotient, remainder};
        }

        BigInt operator/(const BigInt& rhs) const {
            if(rhs.isZero()) throw std::runtime_error("Division by zero attempted.");
            if(isZero()) return BigInt("0");
            if(this->abs() < rhs.abs()) return BigInt("0");

            auto [quotient,_] = this->abs().divmod(rhs.abs());
            quotient.isNegative = isNegative != rhs.isNegative;
            return quotient;
        }

        BigInt operator/=(const BigInt& rhs){
            *this = *this / rhs;
            return *this;
        }

        BigInt operator%(const BigInt& rhs) const {
            if(rhs.isZero()) throw std::runtime_error("Modulo by zero attempted.");
            if(isZero()) return BigInt("0");
            if(this->abs() < rhs.abs()) return *this;

            auto [_,remainder] = this->abs().divmod(rhs.abs());
            return remainder;
        }

        BigInt operator%=(const BigInt& rhs){
            *this = *this % rhs;
            return *this;
        }

        BigInt max(const BigInt& rhs) const {
            return (*this > rhs) ? *this : rhs;
        }

        BigInt min(const BigInt& rhs) const {
            return (*this < rhs) ? *this : rhs;
        }

        BigInt abs() const {
            BigInt temp = *this;
            temp.isNegative = false;
            return temp;
        }

        BigInt pow(BigInt exponent) const {
            if (exponent.isZero()) return BigInt(1);
            BigInt base = *this;
            BigInt result(1);

            while (!exponent.isZero()) {
                if (exponent.digits[0] % 2 != 0) result *= base;
                base *= base;
                exponent = exponent / BigInt(2);
            }

            return result;
        }

        BigInt gcd(const BigInt& rhs) const {
            BigInt a,b;
            a = *this;
            b = rhs;

            while(!a.isZero()){
                BigInt temp = a;
                a = b % a;
                b = temp;
            }

            return b;
        }

        BigInt lcm(const BigInt& rhs) const {
            return (*this * rhs).abs() / this->gcd(rhs);
        }

        BigInt factorial() const {
            BigInt temp = *this;
            BigInt exp = *this;
            while(exp > 1){
                temp *= --exp;
            }

            return temp;
        }

        bool isEven() const {
            return digits[0] % 2 == 0;
        }

        bool isOdd() const {
            return digits[0] % 2 != 0;
        }

        BigInt integerSqrt() const {
            if (isNegative)
                throw std::invalid_argument("Cannot compute square root of negative BigInt");
            if (isZero())
                return BigInt(0);

            BigInt x = *this;
            BigInt y(0);
            BigInt p(1);

            while (p * BigInt(4) <= x)
                p *= BigInt(4);

            while (!p.isZero()) {
                if (x >= y + p) {
                    x -= (y + p);
                    y = y / BigInt(2) + p;
                } else {
                    y /= BigInt(2);
                }
                p /= BigInt(4);
            }

            return y;
        }

        bool fitsInInt() const {
            BigInt int64Min(std::numeric_limits<int>::min());
            BigInt int64Max(std::numeric_limits<int>::max());

            return *this >= int64Min && *this <= int64Max;
        }

        bool fitsInInt64() const {
            BigInt int64Min(std::numeric_limits<int64_t>::min());
            BigInt int64Max(std::numeric_limits<int64_t>::max());

            return *this >= int64Min && *this <= int64Max;
        }

        int toInt() const {
            int64_t value = toInt64();
            if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
                throw std::overflow_error("BigInt too large for int");
            }
            return static_cast<int>(value);
        }

        int64_t toInt64() const {
            int64_t value = 0;
            for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
                value = value * 10 + *it;
            }
            return isNegative ? -value : value;
        }

        double toDecimal() const {
            if (*this == BigInt(0)) return 0.0;

            const size_t MAX_DIGITS = 17;
            double result = 0.0;

            size_t len = digits.size();
            size_t i = 0;

            while (i < std::min(len, MAX_DIGITS)) {
                result = result * 10 + digits[i];
                ++i;
            }

            int exponent = static_cast<int>(len - i);
            result *= std::pow(10.0, exponent);

            return isNegative ? -result : result;
        }

        BigDecimal sqrt(size_t precision = 6) const;
        BigInt operator=(const BigDecimal& rhs);

};
