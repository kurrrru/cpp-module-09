#include <iostream>
#include <string>
#include <stdexcept>

#include <ex01/BigInt.hpp>
#include <ex01/DynamicArray.hpp>

bool BigInt::operator==(const BigInt& rhs) const {
    if (isNegative() != rhs.isNegative()) {
        return false;
    }
    if (_digits.size() != rhs._digits.size()) {
        return false;
    }
    for (int i = _digits.size() - 1; i >= 0; --i) {
        if (_digits[i] != rhs._digits[i]) {
            return false;
        }
    }
    return true;
}

bool BigInt::operator!=(const BigInt& rhs) const {
    return !(*this == rhs);
}

bool BigInt::operator<(const BigInt& rhs) const {
    if (isNegative() != rhs.isNegative()) {
        return isNegative();
    }
    if (isNegative()) {
        if (_digits.size() != rhs._digits.size()) {
            return _digits.size() > rhs._digits.size();
        }
        for (int i = _digits.size() - 1; i >= 0; --i) {
            if (_digits[i] != rhs._digits[i]) {
                return _digits[i] > rhs._digits[i];
            }
        }
        return false;
    } else {
        if (_digits.size() != rhs._digits.size()) {
            return _digits.size() < rhs._digits.size();
        }
        for (int i = _digits.size() - 1; i >= 0; --i) {
            if (_digits[i] != rhs._digits[i]) {
                return _digits[i] < rhs._digits[i];
            }
        }
        return false;
    }
}

bool BigInt::operator<=(const BigInt& rhs) const {
    return (*this < rhs) || (*this == rhs);
}

bool BigInt::operator>(const BigInt& rhs) const {
    return !(*this <= rhs);
}

bool BigInt::operator>=(const BigInt& rhs) const {
    return !(*this < rhs);
}
