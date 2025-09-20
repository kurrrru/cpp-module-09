#include <iostream>
#include <string>

#include <BitcoinExchange.hpp>




int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <data file>" << std::endl;

        // Example: ./btc_exchange input.txt <- これを計算に使う
        return 1;
    }
    try {
        BitcoinExchange btc_exchange("data.csv");
        // Successfully loaded data file
        std::cout << "Data file loaded successfully." << std::endl;
        // Example usage: get exchange rate for a specific date
        toolbox::Date example_date;
        example_date = toolbox::Date(toolbox::GREGORIAN, "2023-01-01", "%Y-%m-%d", true);
        double rate = btc_exchange.get_exchange_rate(example_date);
        std::cout << "Exchange rate on " << example_date.to_string(toolbox::GREGORIAN)
            << " was " << rate << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}