#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>

#include <ex01/BigInt.hpp>
#include <ex01/DynamicArray.hpp>

BigInt::BigInt()
    : _digits(), _isNegative(false) {}

BigInt::BigInt(const BigInt& other)
    : _digits(other._digits), _isNegative(other._isNegative) {}

BigInt& BigInt::operator=(const BigInt& rhs) {
    if (this != &rhs) {
        BigInt tmp(rhs);
        swap(tmp);
    }
    return *this;
}

BigInt::~BigInt() {}

void BigInt::swap(BigInt& other) {
    _digits.swap(other._digits);
    std::swap(_isNegative, other._isNegative);
}

void swap(BigInt& lhs, BigInt& rhs) {
    lhs.swap(rhs);
}

void BigInt::normalize() {
    while (_digits.size() > 1 && _digits[_digits.size() - 1] == 0) {
        _digits.pop_back();
    }
    if (isZero()) {
        _isNegative = false;
    }
}

bool BigInt::isZero() const {
    return _digits.size() == 0 || (_digits.size() == 1 && _digits[0] == 0);
}

bool BigInt::isNegative() const {
    return _isNegative;
}

std::size_t BigInt::size() const {
    return _digits.size();
}

BigInt BigInt::abs() const {
    BigInt result(*this);
    result._isNegative = false;
    return result;
}
