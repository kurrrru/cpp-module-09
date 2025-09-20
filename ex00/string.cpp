#include <sstream>
#include <string>

#include <string.hpp>

namespace toolbox {

std::string to_string(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

int stoi(const std::string &s) {
    std::stringstream ss(s);
    int num;
    ss >> num;
    if (ss.fail() || !ss.eof()) {
        throw std::invalid_argument("Invalid integer: '" + s + "'");
    }
    return num;
}

double stod(const std::string &s) {
    std::stringstream ss(s);
    double num;
    ss >> num;
    if (ss.fail() || !ss.eof()) {
        throw std::invalid_argument("Invalid double: '" + s + "'");
    }
    return num;
}

}  // namespace toolbox
