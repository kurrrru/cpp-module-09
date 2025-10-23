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
    BigInt& operator/=(const BigInt& rhs);
    BigInt& operator%=(const BigInt& rhs);
    BigInt& operator&=(const BigInt& rhs);
    BigInt& operator|=(const BigInt& rhs);
    BigInt& operator^=(const BigInt& rhs);
    BigInt operator-() const;
    BigInt operator~() const;
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
    std::string toString() const;

    std::size_t clz() const;

 private:
    DynamicArray<DigitType> _digits;
    bool _isNegative;
    static const std::size_t BITS_PER_DIGIT = sizeof(DigitType) * 8;
    static const std::size_t MULTIPLY_THRESHOLD = 10;
    static const std::size_t DIVISION_THRESHOLD = 70;

    BigInt karatsuba_multiply(const BigInt& a, const BigInt& b) const;
    BigInt schoolbook_multiply(const BigInt& a, const BigInt& b) const;
    void schoolbook_division(const BigInt& divided,
                                const BigInt& divisor,
                                BigInt& quotient,
                                BigInt& remainder) const;
    void division_and_remainder(const BigInt& divided,
                                const BigInt& divisor,
                                BigInt& quotient,
                                BigInt& remainder) const;
    void recursive_division(const BigInt& divided,
                                const BigInt& divisor,
                                BigInt& quotient,
                                BigInt& remainder) const;
    void divide_2n_by_n(const BigInt& divided,
                        const BigInt& divisor,
                        BigInt& quotient,
                        BigInt& remainder) const;
    void divide_3n_by_2n(const BigInt& divided_high,
                        const BigInt& divided_low,
                        const BigInt& divisor,
                        BigInt& quotient,
                        BigInt& remainder) const;
    BigInt pad_leading_zeros(std::size_t new_size) const;
};

// BigInt_basic.cpp
void swap(BigInt& a, BigInt& b);

// BigInt_calculation.cpp
const BigInt operator+(const BigInt& lhs, const BigInt& rhs);
const BigInt operator-(const BigInt& lhs, const BigInt& rhs);
const BigInt operator*(const BigInt& lhs, const BigInt& rhs);
const BigInt operator/(const BigInt& lhs, const BigInt& rhs);
const BigInt operator%(const BigInt& lhs, const BigInt& rhs);
const BigInt operator&(const BigInt& lhs, const BigInt& rhs);
const BigInt operator|(const BigInt& lhs, const BigInt& rhs);
const BigInt operator^(const BigInt& lhs, const BigInt& rhs);
const BigInt operator<<(const BigInt& num, std::size_t shift);
const BigInt operator>>(const BigInt& num, std::size_t shift);

// BigInt_conversion.cpp
std::ostream& operator<<(std::ostream& os, const BigInt& num);
std::istream& operator>>(std::istream& is, BigInt& num);
