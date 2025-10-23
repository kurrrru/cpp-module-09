#include <ex00/BitcoinExchange.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <map>
#include <utility>

#include <ex00/Date.hpp>
#include <toolbox/string.hpp>

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
        std::cerr << "Warning: failed to open data file: " << data_filename
            << " (the database will be empty)" << std::endl;
        return;
    }
    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "Warning: data file is empty: " << data_filename
            << " (the database will be empty)" << std::endl;
        return;
    }
    if (line != "date,exchange_rate") {
        std::cerr << "Warning: invalid header in data file: " << data_filename
            << "(Expecting 'date,exchange_rate')" << std::endl;
    }
    std::map<toolbox::Date, double> new_map;
    while (std::getline(file, line)) {
        std::size_t pos_delimiter = line.find(",");
        if (pos_delimiter == std::string::npos) {
            std::cerr << "Warning: invalid line format "
                << "(missing delimiter) in data file: "
                << line << "(this line will be ignored)" << std::endl;
            continue;
        }
        std::string date_str = line.substr(0, pos_delimiter);
        std::string value_str = line.substr(pos_delimiter + 1);
        if (date_str.empty() || value_str.empty()) {
            std::cerr << "Warning: invalid line format "
                << "(empty date or value) in data file: "
                << line << "(this line will be ignored)" << std::endl;
            continue;
        }
        toolbox::Date date;
        double value;
        try {
            date = toolbox::Date(toolbox::GREGORIAN, date_str,
                "%Y-%m-%d", true);
            value = toolbox::stod(value_str);
        } catch (const std::exception &e) {
            std::cerr << "Warning: invalid data in line: " << line
                << " (" << e.what() << ") (this line will be ignored)"
                << std::endl;
            continue;
        }
        if (value < 0.0) {
            std::cerr << "Warning: negative exchange rate in line: "
                << line << " (this line will be ignored)" << std::endl;
            continue;
        }
        if (new_map.count(date) > 0) {
            std::cerr << "Warning: duplicate date entry in data file: "
                << line << " (the rate for this date will be overwritten)"
                << std::endl;
        }
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

bool BitcoinExchange::empty() const {
    return _exchange_rates.empty();
}
