#include <calendar_system/EthiopianCalendar.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstring>

#include <calendar_system/JulianCalendar.hpp>
#include <string.hpp>

namespace {

bool is_leap(int year);
int last_day_of_month(int year, int month);
std::string to_string_Ee(int era, bool uppercase);
std::string to_string_Yy(int year, bool uppercase);
std::string to_string_Mm(int month, bool uppercase);
std::string to_string_Dd(int day, bool uppercase);
std::string to_string_Ww(int day_of_week, bool uppercase);

toolbox::JulianCalendar julian;

}  // namespace

namespace toolbox {

EthiopianCalendar::EthiopianCalendar() {
}

EthiopianCalendar::EthiopianCalendar(const EthiopianCalendar& other) {
    (void)other;
}

EthiopianCalendar& EthiopianCalendar::operator=(
    const EthiopianCalendar& other) {
    (void)other;
    return *this;
}

EthiopianCalendar::~EthiopianCalendar() {
}

int EthiopianCalendar::to_serial_date(int era,
        int year, int month, int day) const {
    if (era < 0 || era >= END_OF_ERA) {
        throw std::out_of_range("EthiopianCalendar::to_serial_date failed: "
            "Invalid era: " + toolbox::to_string(era));
    }
    if (year < 0) {
        throw std::out_of_range("EthiopianCalendar::to_serial_date failed: "
            "year must be positive");
    } else if (year == 0) {
        throw std::out_of_range("EthiopianCalendar::to_serial_date failed: "
            "year 0 does not exist in Ethiopian calendar");
    }
    if (era == BC) {
        year = 1 - year;
    }
    if (month < 1 || month > 13) {
        throw std::out_of_range("EthiopianCalendar::to_serial_date failed: "
            "month must be in 1..13");
    }
    int last_day = last_day_of_month(year, month);
    if (day < 1 || day > last_day) {
        throw std::out_of_range("EthiopianCalendar::to_serial_date failed: "
            "day must be in 1.." + toolbox::to_string(last_day));
    }
    const int ethiopian_epoch = julian.to_serial_date(
        toolbox::JulianCalendar::AD, 8, 8, 29);
    // assume year >= 1
    int serial = ethiopian_epoch
        + 365 * (year - 1)
        + (year - 1) / 4
        + 30 * (month - 1)
        + (day - 1);
    return serial;
}

int EthiopianCalendar::to_serial_date(const std::string& date_str,
        const char* format, bool strict) const {
    if (!format) {
        throw std::invalid_argument("EthiopianCalendar::to_serial_date failed: "
            "format is null");
    }
    int era = 0, year = 0, month = 0, day = 0;
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
        throw std::invalid_argument("EthiopianCalendar::to_serial_date failed: "
            "date string does not match the format");
    }
    return to_serial_date(era, year, month, day);
}

void EthiopianCalendar::from_serial_date(int serial_date,
        int& era, int& year, int& month, int& day) const {
    const int ethiopian_epoch = julian.to_serial_date(
        toolbox::JulianCalendar::AD, 8, 8, 29);
    if (serial_date < ethiopian_epoch) {
        throw std::out_of_range("EthiopianCalendar::from_serial_date failed: "
            "serial_date is out of range");
    }
    int z = serial_date - ethiopian_epoch - 365;
    int era_year = (z >= 0 ? z : z - 1460) / 1461;
    int doe = z - era_year * 1461;
    int yoe = (doe - doe / 1460) / 365;
    year = yoe + era_year * 4 + 2;
    int doy = doe - (365 * yoe + yoe / 4);
    month = doy / 30 + 1;
    day = doy % 30 + 1;
    if (year <= 0) {
        era = EthiopianCalendar::BC;
        year = 1 - year;
    } else {
        era = EthiopianCalendar::AD;
    }
}

void EthiopianCalendar::from_serial_date(int serial_date,
        std::string& date_str, const char* format) const {
    if (!format) {
        throw std::invalid_argument(
            "EthiopianCalendar::from_serial_date failed: "
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
                        "EthiopianCalendar::from_serial_date failed: "
                        "Invalid format specifier: %"
                        + toolbox::to_string(format[i]));
            }
        } else {
            ss << format[i];
        }
    }
    date_str = ss.str();
}

void EthiopianCalendar::from_serial_date(int serial_date,
        int& day_of_week) const {
    day_of_week = serial_date >= -4 ?
            (serial_date + 4) % 7 : (serial_date + 5) % 7 + 6;
}

void EthiopianCalendar::parse_formatted_date(
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
                    "EthiopianCalendar::parse_formatted_date failed: "
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
                    "EthiopianCalendar::parse_formatted_date failed: "
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
                    "EthiopianCalendar::parse_formatted_date failed: "
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
                    "EthiopianCalendar::parse_formatted_date failed: "
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
                    "EthiopianCalendar::parse_formatted_date failed: "
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
                "EthiopianCalendar::parse_formatted_date failed: "
                "Invalid format specifier: %" + toolbox::to_string(format[1]));
    }
}

void EthiopianCalendar::parse_Ee(const std::string& date_str,
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
        /* [toolbox::EthiopianCalendar::BC] = */ "B.C.",
        /* [toolbox::EthiopianCalendar::AD] = */ "A.D.",
    };
    const char* era_str_e[] = {
        /* [toolbox::EthiopianCalendar::BC] = */ "BC",
        /* [toolbox::EthiopianCalendar::AD] = */ "AD",
    };
    for (int i = 0; i < EthiopianCalendar::END_OF_ERA; ++i) {
        const char* s = std::isupper(format[1]) ? era_str_E[i] : era_str_e[i];
        std::size_t len = std::strlen(s);
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
        "EthiopianCalendar::parse_Ee failed: "
        "era not found in date_str at position "
        + toolbox::to_string(pos));
}

void EthiopianCalendar::parse_Yy(const std::string& date_str,
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

void EthiopianCalendar::parse_Mm(const std::string& date_str,
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
        const char *month_str_m[] = {
            /* 1  = */ "Meskerem",
            /* 2  = */ "Tikemet",
            /* 3  = */ "Hidar",
            /* 4  = */ "Tahesas",
            /* 5  = */ "Tir",
            /* 6  = */ "Yekatit",
            /* 7  = */ "Megabit",
            /* 8  = */ "Miazia",
            /* 9  = */ "Genbot",
            /* 10 = */ "Sene",
            /* 11 = */ "Hamle",
            /* 12 = */ "Nehase",
            /* 13 = */ "Pagume",
        };
        for (int i = 0; i < 13; ++i) {
            const char* s = month_str_m[i];
            std::size_t len = std::strlen(s);
            if (date_str.compare(pos, len, s) == 0) {
                month = i + 1;
                parse_formatted_date(date_str, pos + len, format + 2,
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
    }
}

void EthiopianCalendar::parse_Dd(const std::string& date_str,
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
    return year % 4 == 3;  // Ethiopian leap year is every 4 years
}

int last_day_of_month(int year, int month) {
    if (month < 1 || month > 13) {
        throw std::out_of_range("last_day_of_month failed: "
            "month must be in 1..13");
    }
    if (month == 13) {
        return is_leap(year) ? 6 : 5;
    }
    return 30;
}

std::string to_string_Ee(int era, bool uppercase) {
    const char* era_str_E[] = {
        /* [toolbox::EthiopianCalendar::BC] = */ "B.C.",
        /* [toolbox::EthiopianCalendar::AD] = */ "A.D.",
    };
    const char* era_str_e[] = {
        /* [toolbox::EthiopianCalendar::BC] = */ "BC",
        /* [toolbox::EthiopianCalendar::AD] = */ "AD",
    };
    if (era < 0 || era >= toolbox::EthiopianCalendar::END_OF_ERA) {
        throw std::out_of_range(
            "to_string_Ee failed: Invalid era: " + toolbox::to_string(era));
    }
    return uppercase ? era_str_E[era] : era_str_e[era];
}

std::string to_string_Yy(int year, bool uppercase) {
    std::ostringstream oss;
    if (year < 0) {
        throw std::out_of_range("to_string_Yy failed: year must be positive");
    } else if (year == 0) {
        throw std::out_of_range("to_string_Yy failed: "
            "year 0 does not exist in Ethiopian calendar");
    }
    if (uppercase) {
        oss << year;
    } else {
        oss << std::setw(4) << std::setfill('0') << year;
    }
    return oss.str();
}

std::string to_string_Mm(int month, bool uppercase) {
    const char *month_str_m[] = {
        /* 1  = */ "Meskerem",
        /* 2  = */ "Tikemet",
        /* 3  = */ "Hidar",
        /* 4  = */ "Tahesas",
        /* 5  = */ "Tir",
        /* 6  = */ "Yekatit",
        /* 7  = */ "Megabit",
        /* 8  = */ "Miazia",
        /* 9  = */ "Genbot",
        /* 10 = */ "Sene",
        /* 11 = */ "Hamle",
        /* 12 = */ "Nehase",
        /* 13 = */ "Pagume",
    };
    if (month < 1 || month > 13) {
        throw std::out_of_range("to_string_Mm failed: "
            "month must be in 1..13");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << month;
    } else {
        oss << month_str_m[month - 1];
    }
    return oss.str();
}

std::string to_string_Dd(int day, bool uppercase) {
    if (day < 1 || day > 30) {
        throw std::out_of_range("to_string_Dd failed: day must be in 1..30");
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
        /* [0] = */ "Ehud",
        /* [1] = */ "Segno",
        /* [2] = */ "Maksegno",
        /* [3] = */ "Erob",
        /* [4] = */ "Hamus",
        /* [5] = */ "Arb",
        /* [6] = */ "Kidame",
    };
    const char* day_of_week_str_w[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };
    if (day_of_week < 0 || day_of_week > 6) {
        throw std::out_of_range("to_string_Ww: day_of_week must be in 0..6");
    }
    return uppercase ?
        day_of_week_str_W[day_of_week] : day_of_week_str_w[day_of_week];
}

}  // namespace
