#include <ex00/conversion.hpp>

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>

namespace {
bool retrieve_date_and_value(const std::string &line,
    std::string &date_str, std::string &value_str);
}  // namespace

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
        std::cerr << "Warning: invalid header in input file: " << file_name
            << " (Expecting 'date | value')" << std::endl;
    }
    while (std::getline(file, line)) {
        std::string date_str, value_str;
        if (!retrieve_date_and_value(line, date_str, value_str)) {
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
                << date.to_string(toolbox::GREGORIAN, "%Y-%m-%d");
            if (btc.empty()) {
                std::cerr << " (exchange rate data is empty)";
            }
            std::cerr << std::endl;
        }
    }
}

namespace {
bool retrieve_date_and_value(const std::string &line,
    std::string &date_str, std::string &value_str) {
    std::size_t pos_delimiter = line.find("|");
    if (pos_delimiter == std::string::npos) {
        std::cerr << "Error: bad input => " << line << std::endl;
        return false;
    }
    std::size_t date_str_end = pos_delimiter;
    while (date_str_end > 0 && line[date_str_end - 1] == ' ') {
        --date_str_end;
    }
    date_str = line.substr(0, date_str_end);
    std::size_t value_str_start = pos_delimiter + 1;
    while (value_str_start < line.size() &&
        line[value_str_start] == ' ') {
        ++value_str_start;
    }
    std::size_t value_str_end = line.size();
    while (value_str_end > value_str_start &&
        line[value_str_end - 1] == ' ') {
        --value_str_end;
    }
    value_str = line.substr(value_str_start, value_str_end - value_str_start);
    return true;
}
}  // namespace
