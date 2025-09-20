#include <Date.hpp>

#include <stdexcept>
#include <string>
#include <ctime>

#include <calendar_system/GregorianCalendar.hpp>
#include <calendar_system/NonProlepticGregorianCalendar.hpp>
#include <calendar_system/JulianCalendar.hpp>
#include <calendar_system/EthiopianCalendar.hpp>
#include <calendar_system/JapaneseWarekiCalendar.hpp>
#include <calendar_system/FrenchRepublicanCalendar.hpp>

namespace {
toolbox::GregorianCalendar gregorian_calendar;
toolbox::NonProlepticGregorianCalendar non_proleptic_gregorian_calendar;
toolbox::JulianCalendar julian_calendar;
toolbox::EthiopianCalendar ethiopian_calendar;
toolbox::FrenchRepublicanCalendar french_republican_calendar;
// toolbox::JapaneseWarekiCalendar japanese_wareki_calendar;
}

toolbox::Date::Date() : _serial_date(0) {}

toolbox::Date::Date(const Date& other) : _serial_date(other._serial_date) {}

toolbox::Date& toolbox::Date::operator=(const Date& other) {
    if (this != &other) {
        _serial_date = other._serial_date;
    }
    return *this;
}

toolbox::Date::~Date() {}

toolbox::Date toolbox::Date::today() {
    std::time_t now_sec = std::time(NULL);
    if (now_sec == static_cast<std::time_t>(-1)) {
        throw std::runtime_error(
            "Date::today failed: Unable to get current time");
    }
    struct std::tm result_tm;

    #ifdef _WIN32
        // Windows
        if (localtime_s(&result_tm, &now_sec) != 0) {
            throw std::runtime_error("Date::today failed: localtime_s failed");
        }
    #else
        // POSIX compliant systems
        if (localtime_r(&now_sec, &result_tm) == NULL) {
            throw std::runtime_error("Date::today failed: localtime_r failed");
        }
    #endif

    int year = result_tm.tm_year + 1900;
    int month = result_tm.tm_mon + 1;
    int day = result_tm.tm_mday;

    int era = toolbox::GregorianCalendar::AD;
    if (year <= 0) {
        era = toolbox::GregorianCalendar::BC;
        year = 1 - year;
    }
    return Date(toolbox::GREGORIAN, era, year, month, day);
}

toolbox::Date::Date(int serial_date) : _serial_date(serial_date) {}

toolbox::Date::Date(toolbox::CalendarSystem cal_sys, int era,
        int year, int month, int day) {
    _serial_date = convert_to_serial_date(cal_sys, era, year, month, day);
}

toolbox::Date::Date(toolbox::CalendarSystem cal_sys,
        const std::string& date_str, const char* format, bool strict) {
    if (!format) {
        throw std::invalid_argument("Date::Date failed: format is null");
    }
    _serial_date = convert_to_serial_date(cal_sys, date_str, format, strict);
}

std::string toolbox::Date::to_string(CalendarSystem cal_sys,
        const char* format) const {
    if (!format) {
        throw std::invalid_argument("Date::to_string failed: format is null");
    }
    std::string date_str;
    convert_from_serial_date(cal_sys, date_str, format);
    return date_str;
}

int toolbox::Date::get_raw_date() const {
    return _serial_date;
}

int toolbox::Date::get_day(toolbox::CalendarSystem cal_sys) const {
    int era, year, month, day;
    convert_form_serial_date(cal_sys, era, year, month, day);
    return day;
}

int toolbox::Date::get_month(toolbox::CalendarSystem cal_sys) const {
    int era, year, month, day;
    convert_form_serial_date(cal_sys, era, year, month, day);
    return month;
}

int toolbox::Date::get_year(toolbox::CalendarSystem cal_sys) const {
    int era, year, month, day;
    convert_form_serial_date(cal_sys, era, year, month, day);
    return year;
}

int toolbox::Date::get_weekday(toolbox::CalendarSystem cal_sys) const {
    int day_of_week;
    convert_from_serial_date(cal_sys, day_of_week);
    return day_of_week;
}

toolbox::Date& toolbox::Date::operator++() {
    ++_serial_date;
    return *this;
}

toolbox::Date toolbox::Date::operator++(int) {
    Date old(*this);
    ++_serial_date;
    return old;
}

toolbox::Date& toolbox::Date::operator--() {
    --_serial_date;
    return *this;
}

toolbox::Date toolbox::Date::operator--(int) {
    Date old(*this);
    --_serial_date;
    return old;
}

toolbox::Date toolbox::Date::operator+(const int delta) const {
    return Date(_serial_date + delta);
}

toolbox::Date toolbox::Date::operator-(const int delta) const {
    return Date(_serial_date - delta);
}

int toolbox::Date::operator-(const Date& other) const {
    return _serial_date - other._serial_date;
}

toolbox::Date& toolbox::Date::operator+=(const int delta) {
    _serial_date += delta;
    return *this;
}

toolbox::Date& toolbox::Date::operator-=(const int delta) {
    _serial_date -= delta;
    return *this;
}

bool toolbox::Date::operator==(const Date& other) const {
    return _serial_date == other._serial_date;
}

bool toolbox::Date::operator!=(const Date& other) const {
    return !(*this == other);
}

bool toolbox::Date::operator<(const Date& other) const {
    return _serial_date < other._serial_date;
}

bool toolbox::Date::operator<=(const Date& other) const {
    return (*this < other) || (*this == other);
}

bool toolbox::Date::operator>(const Date& other) const {
    return !(*this <= other);
}

bool toolbox::Date::operator>=(const Date& other) const {
    return !(*this < other);
}

void toolbox::Date::convert_form_serial_date(toolbox::CalendarSystem cal_sys,
        int& era, int& year, int& month, int& day) const {
    ICalendarSystem& calendar_system = get_calendar_system(cal_sys);
    calendar_system.from_serial_date(_serial_date, era, year, month, day);
}

void toolbox::Date::convert_from_serial_date(toolbox::CalendarSystem cal_sys,
        std::string& date_str, const char* format) const {
    if (!format) {
        throw std::invalid_argument(
            "Date::convert_from_serial_date failed: format is null");
    }
    ICalendarSystem& calendar_system = get_calendar_system(cal_sys);
    calendar_system.from_serial_date(_serial_date, date_str, format);
}

void toolbox::Date::convert_from_serial_date(toolbox::CalendarSystem cal_sys,
        int& day_of_week) const {
    ICalendarSystem& calendar_system = get_calendar_system(cal_sys);
    calendar_system.from_serial_date(_serial_date, day_of_week);
}

int toolbox::Date::convert_to_serial_date(toolbox::CalendarSystem cal_sys,
        int era, int year, int month, int day) const {
    ICalendarSystem& calendar_system = get_calendar_system(cal_sys);
    return calendar_system.to_serial_date(era, year, month, day);
}

int toolbox::Date::convert_to_serial_date(toolbox::CalendarSystem cal_sys,
        const std::string& date_str, const char* format, bool strict) const {
    if (!format) {
        throw std::invalid_argument(
            "Date::convert_to_serial_date failed: format is null");
    }
    ICalendarSystem& calendar_system = get_calendar_system(cal_sys);
    return calendar_system.to_serial_date(date_str, format, strict);
}

// When adding a new calendar system, add it here.
toolbox::ICalendarSystem& toolbox::Date::get_calendar_system(
        toolbox::CalendarSystem cal_sys) const {
    switch (cal_sys) {
        case toolbox::GREGORIAN:
            return gregorian_calendar;
        case toolbox::NON_PROLEPTIC_GREGORIAN:
            return non_proleptic_gregorian_calendar;
        case toolbox::JULIAN:
            return julian_calendar;
        case toolbox::ETHIOPIAN:
            return ethiopian_calendar;
        case toolbox::FRENCH_REPUBLICAN:
            return french_republican_calendar;
        // case toolbox::JAPANESE_WAREKI:
        //     return japanese_wareki_calendar;
        default:
            throw std::invalid_argument("Invalid calendar system");
    }
}
