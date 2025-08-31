#pragma once

namespace toolbox
{

namespace math
{

/**
* @brief Computes the greatest common divisor of two numbers.
* @param a The first number.
* @param b The second number.
* @return The greatest common divisor of a and b.
* @note [constraint]: a != 0 || b != 0
* @note [complexity]: O(log(min(a, b)))
*/
template <typename T>
T gcd(T a, T b) {
    while (b) {
        T t = a % b;
        a = b;
        b = t;
    }
    return a;
}

} // namespace math

} // namespace toolbox
