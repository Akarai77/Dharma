#include "util.hpp"
#include <exception>

class Return : public std::exception {
    public:
        RuntimeValue value;

        const char* what() const noexcept override {
            return "return";
        }

        Return(RuntimeValue& value) : value(value) {}
};
