#pragma once

#include <string>
#include <iostream>
#include <utility>

#include <ex01/DynamicArray.hpp>


class BigInt {
 public:
    typedef unsigned int DigitType;
    // BigInt_basic.cpp
    BigInt();
    BigInt(const BigInt& other);
    BigInt& operator=(const BigInt& rhs);
    ~BigInt();
    void swap(BigInt& other);
    void normalize();
    bool isZero() const;
    bool isNegative() const;
    std::size_t size() const;
    BigInt abs() const;

    // BigInt_calculation.cpp
    BigInt& operator+=(const BigInt& rhs);
    BigInt& operator-=(const BigInt& rhs);
    BigInt& operator*=(const BigInt& rhs);
    BigInt& operator/=(const BigInt& rhs);  // not implemented yet
    BigInt& operator%=(const BigInt& rhs);
    BigInt operator-() const;
    BigInt& operator++();
    BigInt operator++(int);
    BigInt& operator--();
    BigInt operator--(int);
    BigInt& operator<<=(std::size_t shift);
    BigInt& operator>>=(std::size_t shift);

    // BigInt_comparison.cpp
    bool operator==(const BigInt& rhs) const;
    bool operator!=(const BigInt& rhs) const;
    bool operator<(const BigInt& rhs) const;
    bool operator<=(const BigInt& rhs) const;
    bool operator>(const BigInt& rhs) const;
    bool operator>=(const BigInt& rhs) const;

    // BigInt_conversion.cpp
    explicit BigInt(int value);
    explicit BigInt(const std::string& str);
    std::string toString() const;  // not implemented yet

 private:
    DynamicArray<DigitType> _digits;
    bool _isNegative;
    static const std::size_t BITS_PER_DIGIT = sizeof(DigitType) * 8;

    BigInt karatsuba_multiply(const BigInt& a, const BigInt& b) const;
    BigInt schoolbook_multiply(const BigInt& a, const BigInt& b) const;
    void divide_and_remainder(const BigInt& dividend, const BigInt& divisor,
        BigInt& quotient, BigInt& remainder) const;
};

// BigInt_basic.cpp
void swap(BigInt& a, BigInt& b);

// BigInt_calculation.cpp
const BigInt operator+(const BigInt& lhs, const BigInt& rhs);
const BigInt operator-(const BigInt& lhs, const BigInt& rhs);
const BigInt operator*(const BigInt& lhs, const BigInt& rhs);
const BigInt operator/(const BigInt& lhs, const BigInt& rhs);
const BigInt operator%(const BigInt& lhs, const BigInt& rhs);
const BigInt operator<<(const BigInt& num, std::size_t shift);
const BigInt operator>>(const BigInt& num, std::size_t shift);

// BigInt_conversion.cpp
std::ostream& operator<<(std::ostream& os, const BigInt& num);
std::istream& operator>>(std::istream& is, BigInt& num);

