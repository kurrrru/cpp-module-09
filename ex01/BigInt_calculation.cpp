#include <algorithm>
#include <limits>
#include <iostream>
#include <string>
#include <stdexcept>

#include <ex01/BigInt.hpp>
#include <ex01/DynamicArray.hpp>

namespace {

BigInt mask(std::size_t bits) {
    return (BigInt(1) << bits) - BigInt(1);
}

}  // namespace

BigInt& BigInt::operator+=(const BigInt& rhs) {
    if (rhs.isZero()) {
        return *this;
    }
    if (isZero()) {
        *this = rhs;
        return *this;
    }
    if (isNegative() == rhs.isNegative()) {
        bool carry = false;
        for (std::size_t i = 0; i < rhs._digits.size(); ++i) {
            if (i < _digits.size()) {
                _digits[i] += rhs._digits[i] + (carry ? 1 : 0);
                if ((carry && _digits[i] <= rhs._digits[i])
                    || _digits[i] < rhs._digits[i]) {
                    carry = true;
                } else {
                    carry = false;
                }
            } else {
                _digits.push_back(rhs._digits[i] + (carry ? 1 : 0));
                if (carry && _digits[i] == 0) {
                    carry = true;
                } else {
                    carry = false;
                }
            }
        }
        std::size_t i = rhs._digits.size();
        while (carry && i < _digits.size()) {
            _digits[i] += 1;
            if (_digits[i] == 0) {
                carry = true;
            } else {
                carry = false;
            }
            ++i;
        }
        if (carry) {
            _digits.push_back(1);
        }
    } else {
        if (!isNegative()) {
            *this -= (-rhs);
        } else {
            BigInt abs_this = -(*this);
            *this = rhs;
            *this -= abs_this;
        }
    }
    normalize();
    return *this;
}

BigInt& BigInt::operator-=(const BigInt& rhs) {
    if (rhs.isZero()) {
        return *this;
    }
    if (isZero()) {
        *this = -rhs;
        return *this;
    }
    if (isNegative() != rhs.isNegative()) {
        *this += -rhs;
    } else {
        if ((!isNegative() && *this < rhs) || (isNegative() && *this > rhs)) {
            BigInt temp = rhs;
            temp -= *this;
            *this = -temp;
        } else {
            bool borrow = false;
            for (std::size_t i = 0; i < rhs._digits.size(); ++i) {
                if (borrow) {
                    borrow = (_digits[i] <= rhs._digits[i]);
                    _digits[i] -= rhs._digits[i] + 1;
                } else {
                    borrow = (_digits[i] < rhs._digits[i]);
                    _digits[i] -= rhs._digits[i];
                }
            }
            std::size_t i = rhs._digits.size();
            while (borrow && i < _digits.size()) {
                if (_digits[i] == 0) {
                    _digits[i] = std::numeric_limits<DigitType>::max();
                    borrow = true;
                } else {
                    _digits[i] -= 1;
                    borrow = false;
                }
                ++i;
            }
        }
    }
    normalize();
    return *this;
}

BigInt& BigInt::operator*=(const BigInt& rhs) {
    *this = karatsuba_multiply(*this, rhs);
    return *this;
}

BigInt& BigInt::operator/=(const BigInt& rhs) {  // O(n^2)
    BigInt quotient, remainder;
    division_and_remainder(*this, rhs, quotient, remainder);
    *this = quotient;
    return *this;
}

BigInt& BigInt::operator%=(const BigInt& rhs) {  // O(n^2)
    BigInt quotient, remainder;
    division_and_remainder(*this, rhs, quotient, remainder);
    *this = remainder;
    return *this;
}

BigInt& BigInt::operator&=(const BigInt& rhs) {
    BigInt result;
    std::size_t minSize = std::min(this->size(), rhs.size());
    result._digits.resize(minSize);
    for (std::size_t i = 0; i < minSize; ++i) {
        result._digits[i] = this->_digits[i] & rhs._digits[i];
    }
    result._isNegative = this->_isNegative && rhs._isNegative;
    result.normalize();
    *this = result;
    return *this;
}

BigInt& BigInt::operator|=(const BigInt& rhs) {
    BigInt result;
    std::size_t maxSize = std::max(this->size(), rhs.size());
    result._digits.resize(maxSize);
    for (std::size_t i = 0; i < maxSize; ++i) {
        DigitType lhsDigit = (i < this->_digits.size()) ? this->_digits[i] : 0;
        DigitType rhsDigit = (i < rhs._digits.size()) ? rhs._digits[i] : 0;
        result._digits[i] = lhsDigit | rhsDigit;
    }
    result._isNegative = this->_isNegative || rhs._isNegative;
    result.normalize();
    *this = result;
    return *this;
}

BigInt& BigInt::operator^=(const BigInt& rhs) {
    BigInt result;
    std::size_t maxSize = std::max(this->size(), rhs.size());
    result._digits.resize(maxSize);
    for (std::size_t i = 0; i < maxSize; ++i) {
        DigitType lhsDigit = (i < this->_digits.size()) ? this->_digits[i] : 0;
        DigitType rhsDigit = (i < rhs._digits.size()) ? rhs._digits[i] : 0;
        result._digits[i] = lhsDigit ^ rhsDigit;
    }
    result._isNegative = this->_isNegative ^ rhs._isNegative;
    result.normalize();
    *this = result;
    return *this;
}

BigInt& BigInt::operator<<=(std::size_t shift) {
    if (shift == 0 || isZero()) {
        return *this;
    }
    std::size_t digitShift = shift / BITS_PER_DIGIT;
    std::size_t bitShift = shift % BITS_PER_DIGIT;
    if (digitShift > 0) {
        std::size_t oldSize = _digits.size();
        _digits.resize(oldSize + digitShift);
        for (std::size_t i = oldSize; i > 0; --i) {
            _digits[i - 1 + digitShift] = _digits[i - 1];
        }
        for (std::size_t i = 0; i < digitShift; ++i) {
            _digits[i] = 0;
        }
    }
    if (bitShift > 0) {
        std::size_t n = _digits.size();
        DigitType carry = 0;
        const std::size_t rightShift = BITS_PER_DIGIT - bitShift;
        for (std::size_t i = 0; i < n; ++i) {
            DigitType current = _digits[i];
            DigitType newCarry = (current >> rightShift);
            _digits[i] = (current << bitShift) | carry;
            carry = newCarry;
        }
        if (carry > 0) {
            _digits.push_back(carry);
        }
    }
    return *this;
}

BigInt& BigInt::operator>>=(std::size_t shift) {
    if (shift == 0 || isZero()) {
        return *this;
    }
    std::size_t digitShift = shift / BITS_PER_DIGIT;
    std::size_t bitShift = shift % BITS_PER_DIGIT;
    if (digitShift > 0) {
        if (digitShift >= _digits.size()) {
            _digits.clear();
            normalize();
            return *this;
        }
        std::size_t newSize = _digits.size() - digitShift;
        for (std::size_t i = 0; i < newSize; ++i) {
            _digits[i] = _digits[i + digitShift];
        }
        _digits.resize(newSize);
    }
    if (bitShift > 0) {
        std::size_t n = _digits.size();
        if (n == 0) {
            normalize();
            return *this;
        }
        DigitType carry = 0;
        const std::size_t leftShift = BITS_PER_DIGIT - bitShift;
        for (std::size_t i = n; i > 0; --i) {
            std::size_t idx = i - 1;
            DigitType current = _digits[idx];
            DigitType newCarry = (current << leftShift);
            _digits[idx] = (current >> bitShift) | carry;
            carry = newCarry;
        }
    }
    normalize();
    return *this;
}

BigInt BigInt::operator-() const {
    BigInt result(*this);
    if (!isZero()) {
        result._isNegative = !result._isNegative;
    }
    return result;
}

BigInt BigInt::operator~() const {
    BigInt result;
    result._digits.resize(this->size());
    for (std::size_t i = 0; i < this->size(); ++i) {
        result._digits[i] = ~(this->_digits[i]);
    }
    result._isNegative = !this->_isNegative;
    result.normalize();
    return result;
}

BigInt& BigInt::operator++() {
    *this += BigInt(1);
    return *this;
}

BigInt BigInt::operator++(int) {
    BigInt temp(*this);
    ++(*this);
    return temp;
}

BigInt& BigInt::operator--() {
    *this -= BigInt(1);
    return *this;
}

BigInt BigInt::operator--(int) {
    BigInt temp(*this);
    --(*this);
    return temp;
}

const BigInt operator+(const BigInt& lhs, const BigInt& rhs) {
    BigInt result(lhs);
    result += rhs;
    return result;
}

const BigInt operator-(const BigInt& lhs, const BigInt& rhs) {
    BigInt result(lhs);
    result -= rhs;
    return result;
}

const BigInt operator*(const BigInt& lhs, const BigInt& rhs) {
    BigInt result(lhs);
    result *= rhs;
    return result;
}

const BigInt operator/(const BigInt& lhs, const BigInt& rhs) {
    BigInt result(lhs);
    result /= rhs;
    return result;
}

const BigInt operator%(const BigInt& lhs, const BigInt& rhs) {
    BigInt result(lhs);
    result %= rhs;
    return result;
}

const BigInt operator&(const BigInt& lhs, const BigInt& rhs) {
    BigInt result(lhs);
    result &= rhs;
    return result;
}

const BigInt operator|(const BigInt& lhs, const BigInt& rhs) {
    BigInt result(lhs);
    result |= rhs;
    return result;
}

const BigInt operator^(const BigInt& lhs, const BigInt& rhs) {
    BigInt result(lhs);
    result ^= rhs;
    return result;
}

const BigInt operator<<(const BigInt& num, std::size_t shift) {
    BigInt result(num);
    result <<= shift;
    return result;
}

const BigInt operator>>(const BigInt& num, std::size_t shift) {
    BigInt result(num);
    result >>= shift;
    return result;
}

std::size_t BigInt::clz() const {
    if (isZero()) {
        return BITS_PER_DIGIT;
    }
    DigitType most_significant_digit = _digits[_digits.size() - 1];
    std::size_t count = 0;
    if (BITS_PER_DIGIT == 32) {
        if (!(most_significant_digit & 0xFFFF0000)) {
            count += 16;
            most_significant_digit <<= 16;
        }
        if (!(most_significant_digit & 0xFF000000)) {
            count += 8;
            most_significant_digit <<= 8;
        }
        if (!(most_significant_digit & 0xF0000000)) {
            count += 4;
            most_significant_digit <<= 4;
        }
        if (!(most_significant_digit & 0xC0000000)) {
            count += 2;
            most_significant_digit <<= 2;
        }
        if (!(most_significant_digit & 0x80000000)) {
            count += 1;
        }
    } else {
        for (std::size_t i = 0; i < BITS_PER_DIGIT; ++i) {
            if ((most_significant_digit & (1U << (BITS_PER_DIGIT - 1))) == 0) {
                count++;
                most_significant_digit <<= 1;
            } else {
                break;
            }
        }
    }
    return count;
}

BigInt BigInt::karatsuba_multiply(const BigInt& a, const BigInt& b) const {
    if (a.isZero() || b.isZero()) {
        return BigInt();
    }
    if (a.size() < MULTIPLY_THRESHOLD || b.size() < MULTIPLY_THRESHOLD) {
        return schoolbook_multiply(a, b);
    }
    const bool result_is_negative = a._isNegative != b._isNegative;
    BigInt a_abs = a; a_abs._isNegative = false;
    BigInt b_abs = b; b_abs._isNegative = false;
    std::size_t n = std::max(a_abs.size(), b_abs.size());
    std::size_t m = (n + 1) / 2;
    BigInt a_1, a_0, b_1, b_0;
    std::size_t a0_size = std::min(m, a_abs.size());
    a_0._digits.resize(a0_size);
    for (std::size_t i = 0; i < a0_size; ++i) {
        a_0._digits[i] = a_abs._digits[i];
    }
    if (a_abs.size() > m) {
        std::size_t a1_size = a_abs.size() - m;
        a_1._digits.resize(a1_size);
        for (std::size_t i = 0; i < a1_size; ++i) {
            a_1._digits[i] = a_abs._digits[m + i];
        }
    }
    std::size_t b0_size = std::min(m, b_abs.size());
    b_0._digits.resize(b0_size);
    for (std::size_t i = 0; i < b0_size; ++i) {
        b_0._digits[i] = b_abs._digits[i];
    }
    if (b_abs.size() > m) {
        std::size_t b1_size = b_abs.size() - m;
        b_1._digits.resize(b1_size);
        for (std::size_t i = 0; i < b1_size; ++i) {
            b_1._digits[i] = b_abs._digits[m + i];
        }
    }
    a_1.normalize(); a_0.normalize();
    b_1.normalize(); b_0.normalize();
    BigInt Z2 = karatsuba_multiply(a_1, b_1);
    BigInt Z0 = karatsuba_multiply(a_0, b_0);
    BigInt Z1 = karatsuba_multiply(a_1 + a_0, b_1 + b_0);
    BigInt T1 = Z1 - Z2 - Z0;
    Z2 <<= (2 * m * BITS_PER_DIGIT);
    T1 <<= (m * BITS_PER_DIGIT);
    BigInt result = Z2 + T1 + Z0;
    result._isNegative = result_is_negative;
    result.normalize();
    return result;
}

BigInt BigInt::schoolbook_multiply(const BigInt& a, const BigInt& b) const {
    if (a.isZero() || b.isZero()) {
        return BigInt();
    }
    bool result_is_negative = a._isNegative != b._isNegative;
    BigInt a_abs = a;
    a_abs._isNegative = false;
    BigInt b_abs = b;
    b_abs._isNegative = false;

    const DynamicArray<DigitType>& a_digits = a_abs._digits;
    const DynamicArray<DigitType>& b_digits = b_abs._digits;
    BigInt result;
    result._digits.resize(a_digits.size() + b_digits.size(), 0);
    const std::size_t HALF_BITS = BITS_PER_DIGIT / 2;  // 16
    const DigitType MASK_LO = (1U << HALF_BITS) - 1;  // 0x0000FFFF
    for (std::size_t i = 0; i < a_digits.size(); ++i) {
        DigitType carry = 0;
        const DigitType a_val = a_digits[i];
        const DigitType a_lo = a_val & MASK_LO;
        const DigitType a_hi = a_val >> HALF_BITS;
        for (std::size_t j = 0; j < b_digits.size(); ++j) {
            const DigitType b_val = b_digits[j];
            const DigitType b_lo = b_val & MASK_LO;
            const DigitType b_hi = b_val >> HALF_BITS;
            DigitType p0 = a_lo * b_lo;  // (lo * lo)
            DigitType p1 = a_lo * b_hi;  // (lo * hi)
            DigitType p2 = a_hi * b_lo;  // (hi * lo)
            DigitType p3 = a_hi * b_hi;  // (hi * hi)

            DigitType p1_lo = p1 & MASK_LO;
            DigitType p1_hi = p1 >> HALF_BITS;
            DigitType p2_lo = p2 & MASK_LO;
            DigitType p2_hi = p2 >> HALF_BITS;

            DigitType product_lo = p0;
            DigitType product_hi = p3;

            DigitType mid1 = p1_lo << HALF_BITS;
            DigitType carry1 = p1_hi;
            DigitType old_lo = product_lo;
            product_lo += mid1;
            if (product_lo < old_lo) carry1++;
            product_hi += carry1;

            DigitType mid2 = p2_lo << HALF_BITS;
            DigitType carry2 = p2_hi;
            old_lo = product_lo;
            product_lo += mid2;
            if (product_lo < old_lo) carry2++;
            product_hi += carry2;
            old_lo = product_lo;
            product_lo += result._digits[i + j];
            if (product_lo < old_lo) {
                product_hi++;
            }
            old_lo = product_lo;
            product_lo += carry;
            if (product_lo < old_lo) {
                product_hi++;
            }
            result._digits[i + j] = product_lo;
            carry = product_hi;
        }
        result._digits[i + b_digits.size()] = carry;
    }
    result._isNegative = result_is_negative;
    result.normalize();
    return result;
}

void BigInt::schoolbook_division(const BigInt& divided,
                                const BigInt& divisor,
                                BigInt& quotient,
                                BigInt& remainder) const {
    if (divisor.isZero()) {
        throw std::runtime_error("Error: division by zero");
    }
    if (divided.isZero()) {
        quotient = BigInt();
        remainder = BigInt();
        return;
    }
    BigInt a_abs = divided;
    a_abs._isNegative = false;
    BigInt b_abs = divisor;
    b_abs._isNegative = false;
    if (a_abs < b_abs) {
        quotient = BigInt();
        remainder = divided;
        return;
    }
    quotient = BigInt();
    remainder = BigInt();
    for (std::size_t i = a_abs.size(); i > 0; --i) {
        DigitType current_digit = a_abs._digits[i - 1];
        for (int j = BITS_PER_DIGIT - 1; j >= 0; --j) {
            remainder <<= 1;
            remainder += BigInt((current_digit >> j) & 1);
            quotient <<= 1;
            if (remainder >= b_abs) {
                remainder -= b_abs;
                quotient++;
            }
        }
    }
    if (divided._isNegative != divisor._isNegative) {
        quotient._isNegative = true;
    }
    if (divided._isNegative) {
        remainder._isNegative = true;
    }
    quotient.normalize();
    remainder.normalize();
}

void BigInt::division_and_remainder(const BigInt& divided,
                                const BigInt& divisor,
                                BigInt& quotient,
                                BigInt& remainder) const {
    const std::size_t BZ_OFFSET_THRESHOLD = 4;
    if (divisor.isZero()) {
        throw std::runtime_error("Division by zero");
    }
    if (divided.abs() < divisor.abs()) {
        quotient = BigInt();
        remainder = divided;
        return;
    }
    if (divisor.size() < DIVISION_THRESHOLD ||
        (divided.size() - divisor.size() < BZ_OFFSET_THRESHOLD)) {
        schoolbook_division(divided, divisor, quotient, remainder);
        return;
    }

    std::size_t shift = divisor.clz();
    BigInt normalized_divided = divided.abs() << shift;
    BigInt normalized_divisor = divisor.abs() << shift;

    recursive_division(normalized_divided, normalized_divisor,
        quotient, remainder);

    remainder >>= shift;
    if (divided.isNegative() != divisor.isNegative()) {
        quotient._isNegative = true;
    }
    if (divided.isNegative()) {
        remainder._isNegative = true;
    }
    quotient.normalize();
    remainder.normalize();
}

void BigInt::recursive_division(const BigInt& divided,
                                const BigInt& divisor,
                                BigInt& quotient,
                                BigInt& remainder) const {
    std::size_t m = divided.size();
    std::size_t n = divisor.size();
    if (m < n * 2) {
        BigInt divided_pad = divided.pad_leading_zeros(n * 2);
        divide_2n_by_n(divided_pad, divisor, quotient, remainder);
    } else {
        BigInt divided_high = divided >> (n * BITS_PER_DIGIT);
        BigInt divided_low = divided & mask(n * BITS_PER_DIGIT);
        BigInt q_high, r_high;
        recursive_division(divided_high, divisor, q_high, r_high);

        BigInt combined = (r_high << (n * BITS_PER_DIGIT)) + divided_low;
        BigInt q_low;
        divide_2n_by_n(combined, divisor, q_low, remainder);
        quotient = (q_high << (n * BITS_PER_DIGIT)) + q_low;
    }
}

void BigInt::divide_2n_by_n(const BigInt& divided,
                            const BigInt& divisor,
                            BigInt& quotient,
                            BigInt& remainder) const {
    std::size_t n = divisor.size();
    if (n < DIVISION_THRESHOLD) {
        schoolbook_division(divided, divisor, quotient, remainder);
        return;
    }
    std::size_t n_half = n / 2;
    BigInt A1A2 = divided >> (n * BITS_PER_DIGIT);
    BigInt A3 = (divided >> (n_half * BITS_PER_DIGIT))
        & mask(n_half * BITS_PER_DIGIT);
    BigInt A4 = divided & mask(n_half * BITS_PER_DIGIT);

    BigInt Q1, R1;
    divide_3n_by_2n(A1A2, A3, divisor, Q1, R1);
    BigInt Q2;
    divide_3n_by_2n(R1, A4, divisor, Q2, remainder);
    quotient = (Q1 << (n_half * BITS_PER_DIGIT)) + Q2;
    quotient.normalize();
    remainder.normalize();
}

void BigInt::divide_3n_by_2n(const BigInt& divided_high,
                            const BigInt& divided_low,
                            const BigInt& divisor,
                            BigInt& quotient,
                            BigInt& remainder) const {
    std::size_t n = divisor.size();
    std::size_t n_half = n / 2;

    BigInt B1 = divisor >> (n_half * BITS_PER_DIGIT);
    BigInt B2 = divisor & mask(n_half * BITS_PER_DIGIT);

    BigInt A1 = divided_high >> (n_half * BITS_PER_DIGIT);
    BigInt A2 = divided_high & mask(n_half * BITS_PER_DIGIT);

    BigInt R_high;
    if (A1 < B1) {
        divide_2n_by_n(divided_high, B1, quotient, R_high);
    } else {
        quotient = mask(n - n_half);
        R_high = divided_high + divided_low - (B1 << (n * BITS_PER_DIGIT))
            + (B1 << (n_half * BITS_PER_DIGIT));
    }

    remainder = R_high - (quotient * B2);
    while (remainder.isNegative()) {
        remainder += divisor;
        quotient -= BigInt(1);
    }
}

BigInt BigInt::pad_leading_zeros(std::size_t new_size) const {
    BigInt result(*this);
    if (new_size > result.size()) {
        result._digits.resize(new_size);
    }
    return result;
}
