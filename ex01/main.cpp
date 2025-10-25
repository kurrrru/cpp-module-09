#include <list>
#include <stack>
#include <iostream>
#include <string>
#include <sstream>

#include <ex01/RPN.hpp>
#include <toolbox/StepMark.hpp>

int main(int argc, char **argv) {
    toolbox::logger::StepMark::setLogFile("ex01.log");
    toolbox::logger::StepMark::setLevel(toolbox::logger::INFO);

    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0]
            << " <RPN expression> [delimiter]" << std::endl;
        toolbox::logger::StepMark::warning(
            "Invalid argument count supplied to application");
        return 1;
    }
    const char *default_delimiter = " \t";
    std::string delimiter = (argc == 3) ? argv[2] : default_delimiter;
    if (delimiter.empty()) {
        std::cerr << "Error: Delimiter cannot be empty" << std::endl;
        toolbox::logger::StepMark::error("Delimiter string is empty");
        return 1;
    }
    if (delimiter.find_first_of("0123456789-+*/") != std::string::npos) {
        std::cerr << "Error: Delimiter cannot contain digits or operators"
            << std::endl;
        toolbox::logger::StepMark::error(
            "Delimiter contains digits or operator characters");
        return 1;
    }
    try {
        toolbox::logger::StepMark::info("Beginning RPN evaluation");
        Rational result = solveRPN(argv[1], delimiter);
        std::cout << result << std::endl;
        toolbox::logger::StepMark::info("RPN evaluation finished");
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        toolbox::logger::StepMark::error(
            std::string("Unhandled exception: ") + e.what());
        return 1;
    }
    return 0;
}
