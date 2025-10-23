#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include <ex01/BigInt.hpp>
#include <ex01/DynamicArray.hpp>
#include <toolbox/string.hpp>

BigInt::BigInt(int value)
    : _digits(), _isNegative(false) {
    DigitType absValue;
    if (value >= 0) {
        absValue = static_cast<DigitType>(value);
    } else {
        _isNegative = true;
        absValue = -static_cast<DigitType>(value);
    }
    _digits.push_back(absValue);
}

BigInt::BigInt(const std::string& str)
    : _digits(), _isNegative(false) {
    if (str.empty()) {
        throw std::invalid_argument("Invalid string for BigInt: empty string");
    }
    std::size_t startIndex = 0;
    if (str[0] == '-') {
        _isNegative = true;
        startIndex = 1;
    } else if (str[0] == '+') {
        startIndex = 1;
    }
    if (startIndex == str.size()) {
        throw std::invalid_argument("Invalid string for BigInt: no digits");
    }
    const BigInt TEN(10);
    for (std::size_t i = startIndex; i < str.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
            throw std::invalid_argument(
                "Invalid string for BigInt: non-digit character");
        }
        int digit = str[i] - '0';
        *this *= TEN;
        *this += BigInt(digit);
    }
}

std::string BigInt::toString() const {
    if (isZero()) {
        return "0";
    }
    std::string str;
    BigInt temp = *this;
    BigInt TEN(10);
    while (!temp.isZero()) {
        BigInt quotient, remainder;
        division_and_remainder(temp, TEN, quotient, remainder);
        char digitChar = '0' + remainder._digits[0];
        str.push_back(digitChar);
        temp.swap(quotient);
    }
    if (isNegative()) {
        str.push_back('-');
    }
    std::reverse(str.begin(), str.end());
    return str;
}

std::istream& operator>>(std::istream& is, BigInt& bigint) {
    std::string str;
    is >> str;
    if (is) {
        try {
            bigint = BigInt(str);
        } catch (...) {
            is.setstate(std::ios::failbit);
        }
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const BigInt& num) {
    std::string str = num.toString();
    os << str;
    return os;
}
