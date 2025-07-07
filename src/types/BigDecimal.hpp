#pragma once

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>
#include "BigInt.hpp"

class Integer;
BigDecimal intToDecimal(BigInt,size_t);

class BigDecimal {
    private:
        BigInt integer;
        std::vector<uint8_t> fractionalPart;

        void normalizeFractional(BigDecimal& a, BigDecimal& b) const {
            if (a.fractionalPart.size() > b.fractionalPart.size()) {
                b.fractionalPart.resize(a.fractionalPart.size(), 0);
            } else {
                a.fractionalPart.resize(b.fractionalPart.size(), 0);
            }
        }

    public:
        BigDecimal() {}
        BigDecimal(std::string str) { parseFromString(str); }
        BigDecimal(uint8_t digit) : integer(BigInt(digit)) {}
        BigDecimal(size_t digit) : integer(BigInt(digit)) {}
        BigDecimal(int num) : integer(BigInt(num)) {}
        BigDecimal(int64_t num) : integer(BigInt(num)) {}
        BigDecimal(BigInt num) : integer(num) {}
        BigDecimal(float num) { 
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(20) << num;
            parseFromString(oss.str());
        }
        BigDecimal(double num) { 
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(20) << num;
            parseFromString(oss.str());
        }
        BigDecimal(Integer num);

        void removeTrailingZeros() {
            while (!fractionalPart.empty() && fractionalPart.back() == 0) {
                fractionalPart.pop_back();
            }

            if(integer.isZero() && fractionalPart.empty())
                integer.setSign(false);
        }

        void parseFromString(const std::string& str) {
            int dot = str.find('.');
            fractionalPart.clear();
            
            if (dot != std::string::npos) {
                integer = BigInt(str.substr(0, dot));
                for (size_t i = dot + 1; i < str.size(); i++) {
                    if (!std::isdigit(str[i])) throw std::runtime_error("Invalid digit in BigDecimal string.");
                    fractionalPart.push_back(str[i] - '0');
                }

                if(integer.isZero() && !fractionalPart.empty() && str[0] == '-') {
                    integer.setSign(true);
                }
            } else {
                integer = BigInt(str);
            }

            removeTrailingZeros();
        }

        BigInt truncate() const {
            BigInt temp = this->integer;
            return temp;
        }

        BigDecimal truncate(size_t precision) const {
            BigDecimal result = *this;
            if (result.fractionalPart.size() > precision)
                result.fractionalPart.resize(precision);
            return result;
        }

        friend std::ostream& operator<<(std::ostream& out, const BigDecimal& dec) {
            out << dec.integer.toString();
            if (!dec.fractionalPart.empty()) {
                out << '.';
                for (uint8_t d : dec.fractionalPart) {
                    out << static_cast<int>(d);
                }
            }
            return out;
        }

        std::string toString() const {
            std::string res = integer.toString();
            if (!fractionalPart.empty()) {
                res += '.';
                for (uint8_t d : fractionalPart) {
                    res += static_cast<char>(d + '0');
                }
            }
            return res;
        }

        BigDecimal abs() {
            integer.setSign(false);
            return *this;
        }

        bool isZero() const {
            return (integer.isZero() && fractionalPart.empty());
        }

        BigInt stripDecimal() const {
            BigInt res;
            std::string num = this->toString();
            auto it = std::find(num.begin(),num.end(),'.');
            if(it != num.end())
                num.erase(it);
            return BigInt(num);
        }

        BigDecimal operator-() const {
            BigDecimal temp = *this;
            temp.integer.flipSign();
            return temp;
        }

        BigDecimal operator=(const BigInt& rhs){
            this->integer = rhs;
            return *this;
        }

        bool operator==(const BigDecimal& rhs) const {
            return integer == rhs.integer && fractionalPart == rhs.fractionalPart;
        }

        bool operator!=(const BigDecimal& rhs) const {
            return !(*this == rhs);
        }

        bool operator>(const BigDecimal& rhs) const {
            if (integer != rhs.integer)
                return integer > rhs.integer;

            BigDecimal a = *this, b = rhs;
            normalizeFractional(a, b);

            for (size_t i = 0; i < a.fractionalPart.size(); ++i) {
                if (a.fractionalPart[i] != b.fractionalPart[i]) {
                    return a.fractionalPart[i] > b.fractionalPart[i];
                }
            }
            return false;
        }

        bool operator<(const BigDecimal& rhs) const {
            return !(*this > rhs || *this == rhs);
        }

        bool operator>=(const BigDecimal& rhs) const {
            return (*this > rhs || *this == rhs);
        }

        bool operator<=(const BigDecimal& rhs) const {
            return (*this < rhs || *this == rhs);
        }

        BigDecimal operator+(const BigDecimal& rhs) const {
            if(isZero() && rhs.isZero())
                return BigDecimal(0);
            else if(isZero())
                return rhs;
            else if(rhs.isZero())
                return *this;

            BigDecimal a,b,result;
            a = *this;
            b = rhs;
            normalizeFractional(a,b);
            size_t decimalPointPos = a.fractionalPart.size();
            result = intToDecimal(a.stripDecimal() + b.stripDecimal(),decimalPointPos);
            result.removeTrailingZeros();
            return result;
        }

        BigDecimal operator+=(const BigDecimal& rhs){
            *this = *this + rhs;
            return *this;
        }

        BigDecimal& operator++(){
            *this += BigDecimal(1);
            return *this;
        }

        BigDecimal operator++(int){
            BigDecimal temp = *this;
            ++(*this);
            return temp;
        }

        BigDecimal operator-(const BigDecimal& rhs) const {
            if(isZero() && rhs.isZero())
                return BigDecimal(0);
            else if(isZero())
                return rhs;
            else if(rhs.isZero())
                return *this;

            BigDecimal a,b,result;
            a = *this;
            b = rhs;
            normalizeFractional(a,b);
            size_t decimalPointPos = a.fractionalPart.size();
            result = intToDecimal(a.stripDecimal() - b.stripDecimal(),decimalPointPos);
            result.removeTrailingZeros();
            return result;
        }

        BigDecimal operator-=(const BigDecimal& rhs){
            *this = *this - rhs;
            return *this;
        }

        BigDecimal& operator--(){
            *this -= BigDecimal(1);
            return *this;
        }

        BigDecimal operator--(int){
            BigDecimal temp = *this;
            --(*this);
            return temp;
        }

        BigDecimal operator*(const BigDecimal& rhs) const {
            if(isZero() || rhs.isZero())
                return BigDecimal(0);
            
            BigDecimal a,b,result;
            a = *this;
            b = rhs;
            size_t decimalPointPos = a.fractionalPart.size() + b.fractionalPart.size();
            result = intToDecimal(a.stripDecimal() * b.stripDecimal(),decimalPointPos);
            result.removeTrailingZeros();
            return result;
        }

        BigDecimal operator*=(const BigDecimal& rhs){
            *this = *this * rhs;
            return *this;
        }

        BigDecimal operator/(const BigDecimal& rhs) const {
            if(rhs.isZero()) throw std::runtime_error("Division by zero attempted.");
            if(isZero()) return BigDecimal(0);
            
            BigDecimal a,b,result;
            a = *this;
            b = rhs;
            a.abs();
            b.abs();

            int offset = std::max(a.fractionalPart.size(),b.fractionalPart.size());
            b.fractionalPart.resize(offset,0);
            a.fractionalPart.resize(offset,0);

            BigInt dividend = a.stripDecimal();
            BigInt divisor = b.stripDecimal();
            auto [quotient,remainder] = dividend.divmod(divisor);
            std::string res = quotient.toString() + '.';
            
            int precision = 0;
            int precisionLimit = 100;
            while(!remainder.isZero() && precision < precisionLimit){
                remainder *= BigInt(10);
                auto [q,r] = remainder.divmod(divisor);
                res += q.toString();               
                remainder = r;
                precision++;
            }

            result = BigDecimal(res);
            result.integer.setSign(integer.isPositive() != rhs.integer.isPositive());
            result.removeTrailingZeros();
            return result;
        }

        BigDecimal divideWithPrecision(const BigDecimal& rhs, int limit = 100) const {
            if (rhs.isZero()) throw std::runtime_error("Division by zero attempted.");
            if (this->isZero()) return BigDecimal(0);

            BigDecimal a = *this;
            BigDecimal b = rhs;

            a.abs(); b.abs();

            int offset = std::max(a.fractionalPart.size(), b.fractionalPart.size());
            a.fractionalPart.resize(offset, 0);
            b.fractionalPart.resize(offset, 0);

            BigInt dividend = a.stripDecimal();
            BigInt divisor = b.stripDecimal();
            auto [quotient, remainder] = dividend.divmod(divisor);

            std::string res = quotient.toString() + '.';
            int precision = 0;
            while (!remainder.isZero() && precision < limit) {
                remainder *= BigInt(10);
                auto [q, r] = remainder.divmod(divisor);
                res += q.toString();
                remainder = r;
                ++precision;
            }

            BigDecimal result(res);
            result.integer.setSign(integer.isPositive() != rhs.integer.isPositive());
            result.removeTrailingZeros();
            return result;
        }

        BigDecimal operator/=(const BigDecimal& rhs){
            *this = *this / rhs;
            return *this;
        }

        BigDecimal operator%(const BigDecimal& rhs) const {
            if(rhs.isZero()) throw std::runtime_error("Modulo by zero attempted.");
            if(isZero()) return BigDecimal(0);

            BigDecimal a,b,result;
            a = *this;
            b = rhs;
            a.abs();
            b.abs();

            BigDecimal truncatedQuotient = BigDecimal((a / b).integer);
            result = a - b * truncatedQuotient;
            return result;
        }

        BigDecimal operator%=(const BigDecimal& rhs){
            *this = *this % rhs;
            return *this;
        }

        BigDecimal max(const BigDecimal& rhs) const {
            return (*this > rhs) ? *this : rhs;
        }

        BigDecimal min(const BigDecimal& rhs) const {
            return (*this < rhs) ? *this : rhs;
        }

        BigDecimal pow(BigInt exponent) const {
            if (exponent.isZero()) return BigDecimal(1);
            BigDecimal base = *this;
            BigDecimal result(1);

            while (!exponent.isZero()) {
                if (exponent % 2 != 0) result *= base;
                base *= base;
                exponent /= BigInt(2);
            }

            return result;
        }

        BigDecimal gcd(const BigDecimal& rhs) const {
            BigDecimal a,b;
            a = *this;
            b = rhs;

            while(!a.isZero()){
                BigDecimal temp = a;
                a = b % a;
                b = temp;
            }

            return b;
        }

        BigDecimal lcm(const BigDecimal& rhs) const {
            return (*this * rhs).abs() / this->gcd(rhs);
        }

        BigDecimal sqrt(size_t precision = 6) const {
            if (*this < BigDecimal(0))
                throw std::invalid_argument("Cannot compute square root of negative BigDecimal");

            if (*this == BigDecimal(0) || *this == BigDecimal(1))
                return *this;

            BigDecimal epsilon = BigDecimal("0." + std::string(precision - 1, '0') + "1");
            BigDecimal s = *this;
            BigDecimal x;

            if (s > BigDecimal(1)) {
                BigInt int_part = s.truncate();
                int len = int_part.toString().length();
                x = intToDecimal(BigInt(10).pow((len + 1) / 2), 0);
                if (!s.fractionalPart.empty()) {
                    x = x.divideWithPrecision(BigDecimal(10).pow(s.fractionalPart.size() / 2),precision);
                }
            } else {
                x = s * BigDecimal(10).pow(precision / 2);
            }

            const size_t max_iterations = 50;
            size_t iteration = 0;

            while (iteration < max_iterations) {
                BigDecimal s_div_x = s.divideWithPrecision(x, precision);
                BigDecimal next = (x + s_div_x).divideWithPrecision(BigDecimal(2), precision).truncate(precision);

                if ((x - next).abs() <= epsilon) {
                    return next;
                }
                x = next;
                ++iteration;
            }

            return x.truncate(precision);
        }

        double toDecimal() const {
            double result = integer.toDecimal();

            double fractional = 0.0;
            double divisor = 10.0;

            for (uint8_t digit : fractionalPart) {
                fractional += digit / divisor;
                divisor *= 10.0;
            }

            result += fractional;

            return integer.isPositive() ? result : -result;
        }

};  
