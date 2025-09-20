#include <calendar_system/NonProlepticGregorianCalendar.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstring>

#include <string.hpp>
#include <calendar_system/GregorianCalendar.hpp>

namespace toolbox {

NonProlepticGregorianCalendar::NonProlepticGregorianCalendar() {
}

NonProlepticGregorianCalendar::NonProlepticGregorianCalendar(
    const NonProlepticGregorianCalendar&other) {
    (void)other;
}

NonProlepticGregorianCalendar& NonProlepticGregorianCalendar::operator=(
    const NonProlepticGregorianCalendar& other) {
    (void)other;
    return *this;
}

NonProlepticGregorianCalendar::~NonProlepticGregorianCalendar() {
}

int NonProlepticGregorianCalendar::to_serial_date(int era,
    int year, int month, int day) const {
    GregorianCalendar gc;
    int serial = gc.to_serial_date(era, year, month, day);
    validate_serial_date(serial);
    return serial;
}

int NonProlepticGregorianCalendar::to_serial_date(const std::string& date_str,
    const char* format, bool strict) const {
    if (!format) {
        throw std::invalid_argument(
            "NonProlepticGregorianCalendar::to_serial_date failed: "
            "format is null");
    }
    int era = toolbox::NonProlepticGregorianCalendar::AD;
    int year = 0, month = 0, day = 0;
    bool all_found = false;
    int serial = 0;
    parse_formatted_date(date_str, 0, format,
        era, false,
        year, false,
        month, false,
        day, false,
        all_found,
        serial,
        strict);
    if (!all_found) {
        throw std::invalid_argument(
            "NonProlepticGregorianCalendar::to_serial_date failed: "
            "Something went wrong while parsing date_str");
    }
    return serial;
}

void NonProlepticGregorianCalendar::from_serial_date(int serial_date,
    int& era, int& year, int& month, int& day) const {
    validate_serial_date(serial_date);
    GregorianCalendar gc;
    gc.from_serial_date(serial_date, era, year, month, day);
}

void NonProlepticGregorianCalendar::from_serial_date(int serial_date,
    std::string& date_str, const char* format) const {
    validate_serial_date(serial_date);
    GregorianCalendar gc;
    gc.from_serial_date(serial_date, date_str, format);
}

void NonProlepticGregorianCalendar::from_serial_date(int serial_date,
    int& day_of_week) const {
    validate_serial_date(serial_date);
    GregorianCalendar gc;
    gc.from_serial_date(serial_date, day_of_week);
}

void NonProlepticGregorianCalendar::validate_serial_date(
    int serial_date) const {
    // -141427 is 1582-10-15 in Gregorian calendar
    const int begin_gregorian = -141427;
    if (serial_date < begin_gregorian) {
        throw std::out_of_range(
            "NonProlepticGregorianCalendar::validate_serial_date failed: "
            "Dates before 1582-10-15 does not exist "
            "in non-proleptic Gregorian calendar");
    }
}

void NonProlepticGregorianCalendar::parse_formatted_date(
    const std::string& date_str,
    std::size_t pos,
    const char* format,
    int& era, bool era_found,
    int& year, bool year_found,
    int& month, bool month_found,
    int& day, bool day_found,
    bool& all_found,
    int& serial,
    bool strict) const {
    if (pos >= date_str.size() && !*format) {
        if (year_found && month_found && day_found) {
            try {
                int ret = to_serial_date(era, year, month, day);
                if (!all_found) {
                    serial = ret;
                }
            } catch (std::exception &e) {
                return;
            }
            if (all_found) {
                throw std::invalid_argument(
                    "NonProlepticGregorianCalendar::"
                    "parse_formatted_date failed: "
                    "date_str is ambiguous");
            }
            all_found = true;
        }
        return;
    }
    if (pos >= date_str.size() || !*format) {
        return;
    }
    if (date_str[pos] == format[0] && format[0] != '%') {
        parse_formatted_date(date_str, pos + 1, format + 1,
            era, era_found,
            year, year_found,
            month, month_found,
            day, day_found,
            all_found,
            serial,
            strict);
        return;
    }
    if (format[0] != '%') {
        return;
    }
    switch (format[1]) {
        case 'E':
        case 'e': {
            if (era_found) {
                throw std::invalid_argument(
                    "NonProlepticGregorianCalendar::"
                    "parse_formatted_date failed: "
                    "era is specified multiple times");
            }
            parse_Ee(date_str, pos, format,
                era, era_found,
                year, year_found,
                month, month_found,
                day, day_found,
                all_found,
                serial,
                strict);
            return;
        }
        case 'Y':
        case 'y': {
            if (year_found) {
                throw std::invalid_argument(
                    "NonProlepticGregorianCalendar::"
                    "parse_formatted_date failed: "
                    "year is specified multiple times");
            }
            parse_Yy(date_str, pos, format,
                era, era_found,
                year, year_found,
                month, month_found,
                day, day_found,
                all_found,
                serial,
                strict);
            return;
        }
        case 'M':
        case 'm': {
            if (month_found) {
                throw std::invalid_argument(
                    "NonProlepticGregorianCalendar::"
                    "parse_formatted_date failed: "
                    "month is specified multiple times");
            }
            parse_Mm(date_str, pos, format,
                era, era_found,
                year, year_found,
                month, month_found,
                day, day_found,
                all_found,
                serial,
                strict);
            return;
        }
        case 'D':
        case 'd': {
            if (day_found) {
                throw std::invalid_argument(
                    "NonProlepticGregorianCalendar::"
                    "parse_formatted_date failed: "
                    "day is specified multiple times");
            }
            parse_Dd(date_str, pos, format,
                era, era_found,
                year, year_found,
                month, month_found,
                day, day_found,
                all_found,
                serial,
                strict);
            return;
        }
        case '%': {
            if (date_str[pos] != '%') {
                return;
            }
            parse_formatted_date(date_str, pos + 1, format + 2,
                era, era_found,
                year, year_found,
                month, month_found,
                day, day_found,
                all_found,
                serial,
                strict);
            return;
        }
        default:
            throw std::invalid_argument(
                "NonProlepticGregorianCalendar::"
                "parse_formatted_date failed: "
                "Invalid format specifier: %" + toolbox::to_string(format[1]));
    }
}

void NonProlepticGregorianCalendar::parse_Ee(const std::string& date_str,
    std::size_t pos,
    const char* format,
    int& era, bool era_found,
    int& year, bool year_found,
    int& month, bool month_found,
    int& day, bool day_found,
    bool& all_found,
    int& serial,
    bool strict
) const {
    (void)era_found;
    const char* era_str_E[] = {
        /* [toolbox::GregorianCalendar::BC] = */ "B.C.",
        /* [toolbox::GregorianCalendar::AD] = */ "A.D.",
    };
    const char* era_str_e[] = {
        /* [toolbox::GregorianCalendar::BC] = */ "BC",
        /* [toolbox::GregorianCalendar::AD] = */ "AD",
    };
    for (int i = 0; i < toolbox::GregorianCalendar::END_OF_ERA; ++i) {
        const char* s = std::isupper(format[1]) ? era_str_E[i] : era_str_e[i];
        const std::size_t len = std::strlen(s);
        if (date_str.compare(pos, len, s) == 0) {
            era = i;
            parse_formatted_date(date_str, pos + len, format + 2,
                era, true,
                year, year_found,
                month, month_found,
                day, day_found,
                all_found,
                serial,
                strict);
            return;
        }
    }
    throw std::invalid_argument(
        "NonProlepticGregorianCalendar::parse_Ee failed: "
        "era not found in date_str at position "
        + toolbox::to_string(pos));
}

void NonProlepticGregorianCalendar::parse_Yy(const std::string& date_str,
    std::size_t pos,
    const char* format,
    int& era, bool era_found,
    int& year, bool year_found,
    int& month, bool month_found,
    int& day, bool day_found,
    bool& all_found,
    int& serial,
    bool strict
) const {
    (void)year_found;
    if (std::isupper(format[1])) {
        // %Y
        if (date_str[pos] == '0') {
            return;
        }
        for (std::size_t num_len = 1;
            pos + num_len <= date_str.size(); ++num_len) {
            if (!std::isdigit(date_str[pos + num_len - 1])) {
                break;
            }
            std::string year_str = date_str.substr(pos, num_len);
            try {
                year = toolbox::stoi(year_str);
            } catch (std::exception &e) {
                continue;
            }
            parse_formatted_date(date_str, pos + num_len, format + 2,
                era, era_found,
                year, true,
                month, month_found,
                day, day_found,
                all_found,
                serial,
                strict);
            if (all_found && !strict) {
                return;
            }
        }
    } else {
        // %y
        if (pos + 2 > date_str.size()) {
            return;
        }
        std::string year_str = date_str.substr(pos, 2);
        try {
            year = toolbox::stoi(year_str);
        } catch (std::exception &e) {
            return;
        }
        parse_formatted_date(date_str, pos + 2, format + 2,
            era, era_found,
            year, true,
            month, month_found,
            day, day_found,
            all_found,
            serial,
            strict);
        return;
    }
}

void NonProlepticGregorianCalendar::parse_Mm(const std::string& date_str,
    std::size_t pos,
    const char* format,
    int& era, bool era_found,
    int& year, bool year_found,
    int& month, bool month_found,
    int& day, bool day_found,
    bool& all_found,
    int& serial,
    bool strict
) const {
    (void)month_found;
    if (std::isupper(format[1])) {
        // %M
        if (date_str[pos] == '0') {
            return;
        }
        for (std::size_t num_len = 1;
            pos + num_len <= date_str.size() && num_len <= 2; ++num_len) {
            if (!std::isdigit(date_str[pos + num_len - 1])) {
                break;
            }
            std::string month_str = date_str.substr(pos, num_len);
            try {
                month = toolbox::stoi(month_str);
            } catch (std::exception &e) {
                continue;
            }
            parse_formatted_date(date_str, pos + num_len, format + 2,
                era, era_found,
                year, year_found,
                month, true,
                day, day_found,
                all_found,
                serial,
                strict);
            if (all_found && !strict) {
                return;
            }
        }
    } else {
        // %m
        if (pos + 2 > date_str.size()) {
            return;
        }
        std::string month_str = date_str.substr(pos, 2);
        try {
            month = toolbox::stoi(month_str);
        } catch (std::exception &e) {
            return;
        }
        parse_formatted_date(date_str, pos + 2, format + 2,
            era, era_found,
            year, year_found,
            month, true,
            day, day_found,
            all_found,
            serial,
            strict);
        return;
    }
}

void NonProlepticGregorianCalendar::parse_Dd(const std::string& date_str,
    std::size_t pos,
    const char* format,
    int& era, bool era_found,
    int& year, bool year_found,
    int& month, bool month_found,
    int& day, bool day_found,
    bool& all_found,
    int& serial,
    bool strict
) const {
    (void)day_found;
    if (std::isupper(format[1])) {
        // %D
        if (date_str[pos] == '0') {
            return;
        }
        for (std::size_t num_len = 1;
            pos + num_len <= date_str.size() && num_len <= 2; ++num_len) {
            if (!std::isdigit(date_str[pos + num_len - 1])) {
                break;
            }
            std::string day_str = date_str.substr(pos, num_len);
            try {
                day = toolbox::stoi(day_str);
            } catch (std::exception &e) {
                continue;
            }
            parse_formatted_date(date_str, pos + num_len, format + 2,
                era, era_found,
                year, year_found,
                month, month_found,
                day, true,
                all_found,
                serial,
                strict);
            if (all_found && !strict) {
                return;
            }
        }
    } else {
        // %d
        if (pos + 2 > date_str.size()) {
            return;
        }
        std::string day_str = date_str.substr(pos, 2);
        try {
            day = toolbox::stoi(day_str);
        } catch (std::exception &e) {
            return;
        }
        parse_formatted_date(date_str, pos + 2, format + 2,
            era, era_found,
            year, year_found,
            month, month_found,
            day, true,
            all_found,
            serial,
            strict);
        return;
    }
}

}  // namespace toolbox
