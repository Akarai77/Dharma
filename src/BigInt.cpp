#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class BigInt{
    private:
        std::vector<uint8_t> digits;
        bool isNegative = false;

    public:
        BigInt() : digits(1,0), isNegative(false) {}
        BigInt(const std::string& str) { parseFromString(str); }
        BigInt(int num) { *this = BigInt(std::to_string(num)); }
        BigInt(int64_t num) { *this = BigInt(std::to_string(num)); }

        void parseFromString(const std::string& str){
            digits.clear();
            isNegative = false;
            
            size_t start = 0;
            if(str[0] == '-'){ 
                isNegative = true;
                start = 1;
            }

            for(size_t i=str.size();i>start;i--) {
                char c = str[i-1];
                if(!std::isdigit(c)) throw std::invalid_argument("Invalid digit in BigInt string.");
                digits.push_back(c-'0');
            }

            removeLeadingZeros();
        }

        void removeLeadingZeros(){
            while(digits.size() > 1 && digits.back() == 0){
                digits.pop_back();
            }

            if(digits.size() == 1 && digits[0] == 0){
                isNegative = false;
            }
        }

        bool isZero() const {
            return digits.size() == 1 && digits[0] == 0;
        }

        friend std::ostream& operator<<(std::ostream& out, const BigInt& integer){
            if(integer.isNegative)
                out << "-";

            for(auto it = integer.digits.rbegin(); it != integer.digits.rend();it++)
                out<<static_cast<int>(*it);

            return out;

        }

        bool operator>(const BigInt& rhs) const {
            if (isNegative != rhs.isNegative)
                return !isNegative;

            bool result;
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
            return !(*this > rhs || *this == rhs);
        }

        bool operator==(const BigInt& rhs) const {
            return isNegative == rhs.isNegative && digits == rhs.digits;
        }

        BigInt operator+(const BigInt& rhs) const {
            if(isZero()) return rhs;
            if(rhs.isZero()) return *this;

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
            } else {
                BigInt a = *this, b = rhs;
                a.isNegative = false;
                b.isNegative = false;
                if (a > b) {
                    result = a - b;
                    result.isNegative = this->isNegative;
                } else {
                    result = b - a;
                    result.isNegative = rhs.isNegative;
                }
            }

            result.removeLeadingZeros();
            return result;
        }

        BigInt operator-(const BigInt& rhs) const {
            if(isZero()) {
                BigInt res = rhs;
                res.isNegative = res.isNegative ? false : true;
                return res;
            }
            if(rhs.isZero()) return *this;

            if (isNegative != rhs.isNegative) {
                BigInt temp = rhs;
                temp.isNegative = !rhs.isNegative;
                return *this + temp;
            }

            BigInt a = *this;
            BigInt b = rhs;
            a.isNegative = false;
            b.isNegative = false;

            BigInt result;

            if (a < b) {
                result = b - a;
                result.isNegative = !isNegative;
                return result;
            }

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

        std::pair<BigInt, BigInt> splitAt(size_t index) const {
            BigInt high,low;
            low.digits.assign(digits.begin(),digits.begin()+std::min(index,digits.size()));
            if(index < digits.size())
                high.digits.assign(digits.begin()+index,digits.end());
            high.removeLeadingZeros();
            low.removeLeadingZeros();
            return {high,low};
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
            if(n<=2) return naiveMultiply(rhs);

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

};

int main(){
    BigInt i("-02");
    BigInt j("100");
    std::cout<<i<<" "<<j<<" "<<(i*j);
}
