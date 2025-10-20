#include <ex01/Rational.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

#include <toolbox/math.hpp>
#include <ex01/BigInt.hpp>

namespace {

BigInt gcd(const BigInt& a, const BigInt& b) {
    if (b.isZero()) {
        return a;
    }
    return gcd(b, a % b);
}
}


Rational::Rational() : _numerator(0), _denominator(1) {
}

Rational::Rational(BigInt numerator) : _numerator(numerator),
    _denominator(BigInt(1)) {
}

Rational::Rational(BigInt numerator, BigInt denominator)
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

BigInt Rational::numerator() const {
    return _numerator;
}

BigInt Rational::denominator() const {
    return _denominator;
}

std::pair<BigInt, BigInt> Rational::as_pair() const {
    return std::make_pair(_numerator, _denominator);
}

void Rational::normalize() {
    if (_denominator.isZero()) {
        throw std::runtime_error("Denominator cannot be zero");
    }
    if (_numerator.isZero()) {
        _denominator = BigInt(1);
        return;
    }
    if (_denominator.isNegative()) {
        _numerator = -_numerator;
        _denominator = -_denominator;
    }
    BigInt g = gcd(_numerator.abs(), _denominator);
    _numerator /= g;
    _denominator /= g;
}

Rational Rational::operator+(const Rational &other) const {
    return Rational(_numerator * other._denominator
        + other._numerator * _denominator, _denominator * other._denominator);
}

Rational Rational::operator-(const Rational &other) const {
    return Rational(_numerator * other._denominator
        - other._numerator * _denominator, _denominator * other._denominator);
}

Rational Rational::operator*(const Rational &other) const {
    return Rational(_numerator * other._numerator,
        _denominator * other._denominator);
}

Rational Rational::operator/(const Rational &other) const {
    if (other._numerator.isZero()) {
        throw std::runtime_error("Division by zero");
    }
    return Rational(_numerator * other._denominator,
        _denominator * other._numerator);
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
    return _numerator == other._numerator
        && _denominator == other._denominator;
}

bool Rational::operator!=(const Rational &other) const {
    return !(*this == other);
}

bool Rational::operator<(const Rational &other) const {
    if ((_numerator.isNegative()) != (other._numerator.isNegative())) {
        return _numerator < other._numerator;
    }
    // && (other._numerator.isNegative()) is implicitly satisfied
    if ((_numerator.isNegative())) {
        return -other < -(*this);
    }
    return _numerator * other._denominator
        < other._numerator * _denominator;
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

Rational Rational::operator-() const {
    return Rational(-_numerator, _denominator);
}

Rational Rational::abs() const {
    return Rational(_numerator.abs(), _denominator);
}

std::ostream &operator<<(std::ostream &os, const Rational &r) {
    std::pair<BigInt, BigInt> p = r.as_pair();
    if (p.second == BigInt(1)) {
        os << p.first;
    } else {
        os << p.first << "/" << p.second;
    }
    return os;
}
