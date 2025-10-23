#pragma once

#include <string>

#include <ex01/BigInt.hpp>
#include <ex01/Rational.hpp>

Rational solveRPN(const std::string &rpn_expr, const std::string &delimiter);
