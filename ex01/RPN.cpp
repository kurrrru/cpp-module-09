#include <ex01/RPN.hpp>

#include <list>
#include <stack>
#include <iostream>
#include <string>
#include <sstream>

#include <ex01/BigInt.hpp>
#include <ex01/Rational.hpp>
#include <toolbox/string.hpp>

namespace {

void operator_apply(std::stack<Rational, std::list<Rational> > &st,
    const std::string &op);
void operand_push(std::stack<Rational, std::list<Rational> > &st,
    const std::string &token);

}  // namespace

Rational solveRPN(const std::string &rpn_expr, const std::string &delimiter) {
    std::stack<Rational, std::list<Rational> > st;
    size_t pos = 0;
    while (pos < rpn_expr.size()) {
        size_t next_pos = rpn_expr.find(delimiter, pos);
        std::string token = rpn_expr.substr(pos, next_pos - pos);
        if (token.empty()) {
            pos = next_pos + delimiter.size();
            continue;
        }
        if (token == "+" || token == "-" || token == "*" || token == "/") {
                operator_apply(st, token);
        } else if (token.find_first_not_of("0123456789-+/")
            == std::string::npos) {
            operand_push(st, token);
        } else {
            throw std::invalid_argument("Invalid token '" + token + "'");
        }
        if (next_pos == std::string::npos) {
            break;
        }
        pos = next_pos + delimiter.size();
    }
    if (st.size() != 1) {
        throw std::invalid_argument("Invalid RPN expression");
    }
    return st.top();
}

namespace {

void operator_apply(std::stack<Rational, std::list<Rational> > &st,
        const std::string &op) {
    if (st.size() < 2) {
        throw std::runtime_error("Not enough operands for operator '"
            + op + "'");
    }
    Rational rhs = st.top(); st.pop();
    Rational lhs = st.top(); st.pop();
    if (op == "+") {
        st.push(lhs + rhs);
    } else if (op == "-") {
        st.push(lhs - rhs);
    } else if (op == "*") {
        st.push(lhs * rhs);
    } else if (op == "/") {
        st.push(lhs / rhs);  // this may throw an exception if rhs == 0
    } else {
        throw std::runtime_error("Unknown operator '" + op + "'");
    }
}

void operand_push(std::stack<Rational, std::list<Rational> > &st,
    const std::string &token) {
    size_t slash_pos = token.find('/');
    if (slash_pos == std::string::npos) {
        BigInt num = BigInt(token);
        st.push(Rational(num));
    } else {
        if (slash_pos == 0 || slash_pos == token.size() - 1) {
            throw std::invalid_argument("Invalid rational number format: '"
                + token + "'");
        }
        std::string num_str = token.substr(0, slash_pos);
        std::string denom_str = token.substr(slash_pos + 1);
        if (denom_str.find('/') != std::string::npos) {
            throw std::invalid_argument("Invalid rational number format: '"
                + token + "'");
        }
        BigInt num = BigInt(num_str);
        BigInt denom = BigInt(denom_str);

        // this may throw an exception if denom == 0
        st.push(Rational(num, denom));
    }
}

}  // namespace
