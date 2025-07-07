#include <string>

class Integer;
class BigDecimal;
class Nil{
    public:
        Nil() {}
        ~Nil() {}

        bool operator==(const Nil& rhs) const {
            return true;
        }

        bool operator!=(const Nil& rhs) const {
            return false;
        }
            
        friend std::ostream& operator<<(std::ostream& out,const Nil& val) {
            out << "nil";
            return out;
        }
};
