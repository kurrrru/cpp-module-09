#include <ex01/Rational.hpp>

#include <toolbox/math.hpp>
#include <iostream>
#include <stdexcept>
#include <algorithm>

Rational::Rational() : _numerator(0), _denominator(1) {
}

Rational::Rational(int numerator) : _numerator(numerator), _denominator(1) {
}

Rational::Rational(int numerator, int denominator)
    : _numerator(numerator), _denominator(denominator) {
    normalize();
}

Rational::Rational(const Rational &other)
    : _numerator(other._numerator), _denominator(other._denominator) {
}

Rational &Rational::operator=(const Rational &other) {
    if (this != &other) {
        _numerator = other._numerator;
        _denominator = other._denominator;
    }
    return *this;
}

Rational::~Rational() {
}

int Rational::numerator() const {
    return _numerator;
}

int Rational::denominator() const {
    return _denominator;
}

std::pair<int, int> Rational::as_pair() const {
    return std::make_pair(_numerator, _denominator);
}

void Rational::normalize() {
    if (_denominator == 0) {
        throw std::invalid_argument("Denominator cannot be zero");
    }
    if (_numerator == 0) {
        _denominator = 1;
        return;
    }
    if (_denominator < 0) {
        _numerator = -_numerator;
        _denominator = -_denominator;
    }
    int g = toolbox::math::gcd(std::abs(_numerator), _denominator);
    _numerator /= g;
    _denominator /= g;
}

Rational Rational::operator+(const Rational &other) const {
    return Rational(_numerator * other._denominator + other._numerator * _denominator,
                    _denominator * other._denominator);
}

Rational Rational::operator-(const Rational &other) const {
    return Rational(_numerator * other._denominator - other._numerator * _denominator,
                    _denominator * other._denominator);
}

Rational Rational::operator*(const Rational &other) const {
    return Rational(_numerator * other._numerator, _denominator * other._denominator);
}

Rational Rational::operator/(const Rational &other) const {
    if (other._numerator == 0) {
        throw std::invalid_argument("Division by zero");
    }
    return Rational(_numerator * other._denominator, _denominator * other._numerator);
}

Rational &Rational::operator+=(const Rational &other) {
    *this = *this + other;
    return *this;
}

Rational &Rational::operator-=(const Rational &other) {
    *this = *this - other;
    return *this;
}

Rational &Rational::operator*=(const Rational &other) {
    *this = *this * other;
    return *this;
}

Rational &Rational::operator/=(const Rational &other) {
    *this = *this / other;
    return *this;
}

bool Rational::operator==(const Rational &other) const {
    return _numerator == other._numerator && _denominator == other._denominator;
}

bool Rational::operator!=(const Rational &other) const {
    return !(*this == other);
}

bool Rational::operator<(const Rational &other) const {
    return _numerator * other._denominator < other._numerator * _denominator;
}

bool Rational::operator<=(const Rational &other) const {
    return *this < other || *this == other;
}

bool Rational::operator>(const Rational &other) const {
    return !(*this <= other);
}

bool Rational::operator>=(const Rational &other) const {
    return !(*this < other);
}

std::ostream &operator<<(std::ostream &os, const Rational &r) {
    std::pair<int, int> p = r.as_pair();
    if (p.second == 1) {
        os << p.first;
    } else {
        os << p.first << "/" << p.second;
    }
    return os;
}
