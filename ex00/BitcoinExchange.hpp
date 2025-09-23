#pragma once

#include <map>
#include <string>
#include <stdexcept>
#include <fstream>

#include <ex00/Date.hpp>

class BitcoinExchange {
 public:
    BitcoinExchange();
    BitcoinExchange(const BitcoinExchange &other);
    BitcoinExchange &operator=(const BitcoinExchange &other);
    ~BitcoinExchange();

    explicit BitcoinExchange(const std::string &data_filename);

    void load_data(const std::string &data_filename);
    double get_exchange_rate(const toolbox::Date &date) const;
 private:
    std::map<toolbox::Date, double> _exchange_rates;
};
