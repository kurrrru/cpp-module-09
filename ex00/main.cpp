#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>

#include <ex00/BitcoinExchange.hpp>
#include <ex00/conversion.hpp>
#include <toolbox/string.hpp>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Error: could not open file." << std::endl;
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
