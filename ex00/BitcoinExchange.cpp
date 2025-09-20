#include <BitcoinExchange.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <map>
#include <utility>

#include <Date.hpp>
#include <string.hpp>

BitcoinExchange::BitcoinExchange() : _exchange_rates() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other)
    : _exchange_rates(other._exchange_rates) {}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other) {
    if (this != &other) {
        _exchange_rates = other._exchange_rates;
    }
    return *this;
}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const std::string &data_filename)
    : _exchange_rates() {
    load_data(data_filename);
}

void BitcoinExchange::load_data(const std::string &data_filename) {
    std::ifstream file(data_filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open data file: " + data_filename);
    }
    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Data file is empty: " + data_filename);
    }
    if (line != "date,exchange_rate") {
        throw std::runtime_error("Invalid header in data file: "
            + data_filename);
    }
    std::map<toolbox::Date, double> new_map;
    while (std::getline(file, line)) {
        std::size_t pos_delimiter = line.find(",");
        if (pos_delimiter == std::string::npos) {
            throw std::runtime_error(
                "Invalid line format (missing delimiter) in data file: "
                + line);
        }
        std::string date_str = line.substr(0, pos_delimiter);
        std::string value_str = line.substr(pos_delimiter + 1);
        if (date_str.empty() || value_str.empty()) {
            throw std::runtime_error(
                "Invalid line format (empty date or value) in data file: "
                + line);
        }
        toolbox::Date date(toolbox::GREGORIAN, date_str, "%Y-%m-%d", true);
        double value = toolbox::stod(value_str);
        new_map[date] = value;
    }
    std::swap(_exchange_rates, new_map);
}

double BitcoinExchange::get_exchange_rate(const toolbox::Date &date) const {
    std::map<toolbox::Date, double>::const_iterator it;
    it = _exchange_rates.upper_bound(date);
    if (it == _exchange_rates.begin()) {
        throw std::out_of_range(
            "No exchange rate data available for the given date or earlier");
    }
    --it;
    return it->second;
}
