#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>

#include <ex00/BitcoinExchange.hpp>
#include <ex00/conversion.hpp>
#include <toolbox/string.hpp>
#include <toolbox/StepMark.hpp>

int main(int argc, char **argv) {
    toolbox::logger::StepMark::setLogFile("ex00.log");
    toolbox::logger::StepMark::setLevel(toolbox::logger::INFO);

    if (argc != 2) {
        std::cerr << "Error: could not open file." << std::endl;
        toolbox::logger::StepMark::warning(
            "Invalid number of arguments supplied");
        return 1;
    }
    try {
        toolbox::logger::StepMark::info(
            "Initializing BitcoinExchange with data.csv");
        BitcoinExchange btc_exchange("data.csv");
        toolbox::logger::StepMark::info(std::string(
            "Processing input file: ") + argv[1]);
        convert_and_print(btc_exchange, argv[1]);
        toolbox::logger::StepMark::info(
            "Processing finished without fatal errors");
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        toolbox::logger::StepMark::error(std::string(
            "Unhandled exception: ") + e.what());
        return 1;
    }
    return 0;
}
