#include <calendar_system/GregorianCalendar.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstring>

#include <string.hpp>

namespace {

bool is_leap(int year);
int last_day_of_month(int year, int month);
std::string to_string_Ee(int era, bool uppercase);
std::string to_string_Yy(int year, bool uppercase);
std::string to_string_Mm(int month, bool uppercase);
std::string to_string_Dd(int day, bool uppercase);
std::string to_string_Ww(int day_of_week, bool uppercase);

}  // namespace

namespace toolbox {

GregorianCalendar::GregorianCalendar() {
}

GregorianCalendar::GregorianCalendar(const GregorianCalendar& other) {
    (void)other;
}

GregorianCalendar& GregorianCalendar::operator=(
    const GregorianCalendar& other) {
    (void)other;
    return *this;
}

GregorianCalendar::~GregorianCalendar() {
}

int GregorianCalendar::to_serial_date(int era,
        int year, int month, int day) const {
    if (era < 0 || era >= END_OF_ERA) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed: "
            "Invalid era: " + toolbox::to_string(era));
    }
    if (year < 0) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed: "
            "year must be positive");
    } else if (year == 0) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed: "
            "year 0 does not exist in Gregorian calendar");
    }
    if (era == BC) {
        year = 1 - year;
    }
    if (month < 1 || month > 12) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed: "
            "month must be in 1..12");
    }
    if (day < 1 || day > last_day_of_month(year, month)) {
        throw std::out_of_range("GregorianCalendar::to_serial_date failed: "
            "day is out of range for month " + toolbox::to_string(month)
            + " of year " + toolbox::to_string(year));
    }
    year -= !!(month <= 2);
    const int era_year = (year >= 0 ? year : year - 399) / 400;
    const unsigned int yoe = static_cast<unsigned int>(year - era_year * 400);
    const unsigned int doy = (153 * (month > 2 ? month - 3 : month + 9) + 2)
        / 5 + day - 1;
    const unsigned int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era_year * 146097 + static_cast<int>(doe) - 719468;
}

int GregorianCalendar::to_serial_date(const std::string& date_str,
        const char* format, bool strict) const {
    if (!format) {
        throw std::invalid_argument("GregorianCalendar::to_serial_date failed: "
            "format is null");
    }
    int era = toolbox::GregorianCalendar::AD;
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
        throw std::invalid_argument("GregorianCalendar::to_serial_date failed: "
            "Something went wrong while parsing date_str");
    }
    return serial;
}

void GregorianCalendar::from_serial_date(int serial_date,
        int& era, int& year, int& month, int& day) const {
    (void)era;
    // Hinnant's algorithm
    int z = serial_date + 719468;
    const int era_year = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned int doe = static_cast<unsigned int>(z - era_year * 146097);
    const unsigned int yoe = (doe - doe / 1460 + doe / 36524
        - doe / 146096) / 365;
    year = static_cast<int>(yoe) + era_year * 400;
    const unsigned int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned int mp = (5 * doy + 2) / 153;
    day = doy - (153 * mp + 2) / 5 + 1;
    month = mp < 10 ? mp + 3 : mp - 9;
    year += !!(month <= 2);
    era = year <= 0 ? BC : AD;
    if (year <= 0) {
        year = 1 - year;
    }
}

void GregorianCalendar::from_serial_date(int serial_date,
        std::string& date_str, const char* format) const {
    if (!format) {
        throw std::invalid_argument(
            "GregorianCalendar::from_serial_date failed: "
            "format is null");
    }
    int era, year, month, day;
    from_serial_date(serial_date, era, year, month, day);
    int day_of_week;
    from_serial_date(serial_date, day_of_week);
    std::ostringstream ss;
    for (std::size_t i = 0; format[i]; ++i) {
        if (format[i] == '%') {
            switch (format[++i]) {
                case 'E':
                case 'e':
                    ss << to_string_Ee(era, std::isupper(format[i]));
                    break;
                case 'Y':
                case 'y':
                    ss << to_string_Yy(year, std::isupper(format[i]));
                    break;
                case 'M':
                case 'm':
                    ss << to_string_Mm(month, std::isupper(format[i]));
                    break;
                case 'D':
                case 'd':
                    ss << to_string_Dd(day, std::isupper(format[i]));
                    break;
                case 'W':
                case 'w':
                    ss << to_string_Ww(day_of_week, std::isupper(format[i]));
                    break;
                case '%':
                    ss << '%';
                    break;
                default:
                    throw std::invalid_argument(
                        "GregorianCalendar::from_serial_date failed: "
                        "Invalid format specifier: %"
                        + toolbox::to_string(format[i]));
            }
        } else {
            ss << format[i];
        }
    }
    date_str = ss.str();
}

void GregorianCalendar::from_serial_date(int serial_date,
        int& day_of_week) const {
    day_of_week = serial_date >= -4 ?
            (serial_date + 4) % 7 : (serial_date + 5) % 7 + 6;
}

void GregorianCalendar::parse_formatted_date(const std::string& date_str,
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
                    "GregorianCalendar::parse_formatted_date failed: "
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
                    "GregorianCalendar::parse_formatted_date failed: "
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
                    "GregorianCalendar::parse_formatted_date failed: "
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
                    "GregorianCalendar::parse_formatted_date failed: "
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
                    "GregorianCalendar::parse_formatted_date failed: "
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
                "GregorianCalendar::parse_formatted_date failed: "
                "Invalid format specifier: %" + toolbox::to_string(format[1]));
    }
}

void GregorianCalendar::parse_Ee(const std::string& date_str,
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
        "GregorianCalendar::parse_Ee failed: "
        "era not found in date_str at position "
        + toolbox::to_string(pos));
}

void GregorianCalendar::parse_Yy(const std::string& date_str,
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

void GregorianCalendar::parse_Mm(const std::string& date_str,
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

void GregorianCalendar::parse_Dd(const std::string& date_str,
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

namespace {
bool is_leap(int year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

int last_day_of_month(int year, int month) {
    const int last_day[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    if (month == 2 && is_leap(year)) {
        return 29;
    }
    return last_day[month - 1];
}

std::string to_string_Ee(int era, bool uppercase) {
    const char* era_str_E[] = {
        /* [toolbox::GregorianCalendar::BC] = */ "B.C.",
        /* [toolbox::GregorianCalendar::AD] = */ "A.D.",
    };
    const char* era_str_e[] = {
        /* [toolbox::GregorianCalendar::BC] = */ "BC",
        /* [toolbox::GregorianCalendar::AD] = */ "AD",
    };
    if (era < 0 || era >= toolbox::GregorianCalendar::END_OF_ERA) {
        throw std::out_of_range(
            "to_string_Ee failed: Invalid era: " + toolbox::to_string(era));
    }
    return uppercase ? era_str_E[era] : era_str_e[era];
}

std::string to_string_Yy(int year, bool uppercase) {
    if (year < 0) {
        throw std::out_of_range("to_string_Yy failed: year must be positive");
    } else if (year == 0) {
        throw std::out_of_range("to_string_Yy failed: "
            "year 0 does not exist in Gregorian calendar");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << year;
    } else {
        oss << std::setw(2) << std::setfill('0') << (year % 100);
    }
    return oss.str();
}

std::string to_string_Mm(int month, bool uppercase) {
    if (month < 1 || month > 12) {
        throw std::out_of_range("to_string_Mm failed: month must be in 1..12");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << month;
    } else {
        oss << std::setw(2) << std::setfill('0') << month;
    }
    return oss.str();
}

std::string to_string_Dd(int day, bool uppercase) {
    if (day < 1 || day > 31) {
        throw std::out_of_range("to_string_Dd failed: day must be in 1..31");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << day;
    } else {
        oss << std::setw(2) << std::setfill('0') << day;
    }
    return oss.str();
}

std::string to_string_Ww(int day_of_week, bool uppercase) {
    const char* day_of_week_str_W[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };
    const char* day_of_week_str_w[] = {
        "Sun.", "Mon.", "Tue.", "Wed.",
        "Thu.", "Fri.", "Sat."
    };
    if (day_of_week < 0 || day_of_week > 6) {
        throw std::out_of_range("to_string_Ww: day_of_week must be in 0..6");
    }
    return uppercase ?
        day_of_week_str_W[day_of_week] : day_of_week_str_w[day_of_week];
}

}  // namespace
