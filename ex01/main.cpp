#include <list>
#include <stack>
#include <iostream>
#include <string>
#include <sstream>

#include <ex01/RPN.hpp>

int main(int argc, char **argv) {
    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0]
            << " <RPN expression> [delimiter]" << std::endl;
        return 1;
    }
    std::string delimiter = (argc == 3) ? argv[2] : " ";
    if (delimiter.empty()) {
        std::cerr << "Error: Delimiter cannot be empty" << std::endl;
        return 1;
    }
    if (delimiter.find_first_of("0123456789-+*/") != std::string::npos) {
        std::cerr << "Error: Delimiter cannot contain digits or operators"
            << std::endl;
        return 1;
    }
    try {
        Rational result = solveRPN(argv[1], delimiter);
        std::cout << result << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
