#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <ex00/BitcoinExchange.hpp>
#include <toolbox/string.hpp>

void convert_and_print(const BitcoinExchange &btc,
    const std::string &file_name) {
    std::ifstream file(file_name.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open input file: " + file_name);
    }
    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Input file is empty: " + file_name);
    }
    if (line != "date | value") {
        throw std::runtime_error("Invalid header in input file: " + file_name);
    }
    while (std::getline(file, line)) {
        std::string date_str, delimiter, value_str;
        std::istringstream ss(line);
        if (!(ss >> date_str >> delimiter >> value_str) || !ss.eof()) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }
        if (delimiter != "|") {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }
        toolbox::Date date;
        double value;
        try {
            date = toolbox::Date(toolbox::GREGORIAN, date_str, "%Y-%m-%d");
            value = toolbox::stod(value_str);
        } catch (const std::exception &e) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }
        if (value < 0.0) {
            std::cerr << "Error: not a positive number." << std::endl;
            continue;
        }
        if (value > 1000.0) {
            std::cerr << "Error: too large a number." << std::endl;
            continue;
        }
        try {
            double rate = btc.get_exchange_rate(date);
            double result = value * rate;
            std::cout << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d")
                << " => " << value << " = " << result << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error: no exchange rate available for date: "
                << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d") << std::endl;
        }
    }
}

int main(int argc, char **argv) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <data file>" << std::endl;
        return 1;
    }
    try {
        BitcoinExchange btc_exchange("data.csv");
        convert_and_print(btc_exchange, argv[1]);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
