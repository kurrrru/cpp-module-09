#pragma once

#include <iostream>
#include <utility>

#include <ex01/BigInt.hpp>

class Rational {
 public:
    Rational();
    explicit Rational(BigInt numerator);
    Rational(BigInt numerator, BigInt denominator);
    Rational(const Rational &other);
    Rational &operator=(const Rational &other);
    ~Rational();

    BigInt numerator() const;
    BigInt denominator() const;
    std::pair<BigInt, BigInt> as_pair() const;

    Rational operator+(const Rational &other) const;
    Rational operator-(const Rational &other) const;
    Rational operator*(const Rational &other) const;
    Rational operator/(const Rational &other) const;

    Rational &operator+=(const Rational &other);
    Rational &operator-=(const Rational &other);
    Rational &operator*=(const Rational &other);
    Rational &operator/=(const Rational &other);

    bool operator==(const Rational &other) const;
    bool operator!=(const Rational &other) const;
    bool operator<(const Rational &other) const;
    bool operator<=(const Rational &other) const;
    bool operator>(const Rational &other) const;
    bool operator>=(const Rational &other) const;

    Rational operator-() const;
    Rational abs() const;

 private:
    BigInt _numerator;
    BigInt _denominator;
    void normalize();
};

std::ostream &operator<<(std::ostream &os, const Rational &r);
