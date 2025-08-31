#include <list>
#include <stack>
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>

#include "Rational.hpp"

namespace {

void operator_apply(std::stack<Rational, std::list<Rational> > &st, const std::string &op);
void operand_push(std::stack<Rational, std::list<Rational> > &st, const std::string &token);

int stoi(const std::string &s);

}  // namespace

int main(int argc, char **argv) {
    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <RPN expression>" << " [delimiter]" << std::endl;
        return 1;
    }
    std::string delimiter = (argc == 3) ? argv[2] : " ";
    if (delimiter.empty()) {
        std::cerr << "Error: Delimiter cannot be empty" << std::endl;
        return 1;
    }
    if (delimiter.find_first_of("0123456789-+*/") != std::string::npos) {
        std::cerr << "Error: Delimiter cannot contain digits or operators" << std::endl;
        return 1;
    }
    std::string rpn_expr = argv[1];
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
            try {
                operator_apply(st, token);
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return 1;
            }
        } else if (token.find_first_not_of("0123456789-+/") == std::string::npos) {
            try {
                operand_push(st, token);
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Invalid token '" << token << "'" << std::endl;
            return 1;
        }
        if (next_pos == std::string::npos) {
            break;
        }
        pos = next_pos + delimiter.size();
    }
    if (st.size() != 1) {
        std::cerr << "Error: Invalid RPN expression" << std::endl;
        return 1;
    }
    std::cout << st.top() << std::endl;
    return 0;
}

namespace {
    
void operator_apply(std::stack<Rational, std::list<Rational> > &st, const std::string &op) {
    if (st.size() < 2) {
        throw std::runtime_error("Not enough operands for operator '" + op + "'");
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
        st.push(lhs / rhs); // this may throw an exception if rhs == 0
    } else {
        throw std::runtime_error("Unknown operator '" + op + "'");
    }
}

void operand_push(std::stack<Rational, std::list<Rational> > &st, const std::string &token) {
    size_t slash_pos = token.find('/');
    if (slash_pos == std::string::npos) {
        int num = stoi(token.c_str());
        st.push(Rational(num));
    } else {
        if (slash_pos == 0 || slash_pos == token.size() - 1) {
            throw std::invalid_argument("Invalid rational number format: '" + token + "'");
        }
        std::string num_str = token.substr(0, slash_pos);
        std::string denom_str = token.substr(slash_pos + 1);
        if (denom_str.find('/') != std::string::npos) {
            throw std::invalid_argument("Invalid rational number format: '" + token + "'");
        }
        int num = stoi(token.substr(0, slash_pos).c_str());
        int denom = stoi(token.substr(slash_pos + 1).c_str());
        st.push(Rational(num, denom));  // this may throw an exception if denom == 0
    }
}

int stoi(const std::string &s) {
    std::stringstream ss(s);
    int num;
    ss >> num;
    if (ss.fail() || !ss.eof()) {
        throw std::invalid_argument("Invalid integer: '" + s + "'");
    }
    return num;
}

}  // namespace
