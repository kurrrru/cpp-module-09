#pragma once

#include <string>

#include <ex00/BitcoinExchange.hpp>
#include <toolbox/string.hpp>

void convert_and_print(const BitcoinExchange &btc,
    const std::string &file_name);
