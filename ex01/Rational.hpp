#pragma once

#include <iostream>
#include <utility>

class Rational {
 public:
    Rational();
    explicit Rational(int numerator);
    Rational(int numerator, int denominator);
    Rational(const Rational &other);
    Rational &operator=(const Rational &other);
    ~Rational();

    int numerator() const;
    int denominator() const;
    std::pair<int, int> as_pair() const;

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
    int _numerator;
    int _denominator;
    void normalize();
};

std::ostream &operator<<(std::ostream &os, const Rational &r);
