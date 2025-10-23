/**
 * @file Date.hpp
 * @brief Defines the Date class, a representation of dates supporting multiple
 * calendar systems.
 *
 * The Date class internally uses a serial date number (days since a fixed
 * epoch, like the Unix epoch) to represent a specific point in time. It
 * provides functionalities to convert this serial date to and from various
 * calendar systems (Gregorian, Julian, Ethiopian, French Republican, etc.)
 * through the ICalendarSystem interface. It also supports date arithmetic
 * (addition, subtraction, comparison) and parsing/formatting date strings
 * based on specified formats.
 *
 * ## Adding a New Calendar System
 *
 * To add support for a new calendar system, follow these steps:
 *
 * 1.  **Define the Era Enum (if needed):**
 * In the new calendar system's header file (e.g., `NewCalendar.hpp`), define
 * an `enum Era { ... };` if the calendar uses specific eras (like BC/AD).
 *
 * 2.  **Implement the ICalendarSystem Interface:**
 * Create a new class (e.g., `NewCalendar`) that inherits from
 * `toolbox::ICalendarSystem`.
 * Implement all the pure virtual functions defined in `ICalendarSystem.hpp`:
 * - `to_serial_date(int era, int year, int month, int day) const`:
 *      Converts a date in the new calendar to the common serial date.
 * - `to_serial_date(const std::string& date_str, const char* format,
 *      bool strict) const`:
 *      Parses a date string according to the format and converts it
 *      to the serial date.
 * - `from_serial_date(int serial_date, int& era, int& year,
 *      int& month, int& day) const`:
 *      Converts the common serial date back to the new calendar's
 *      date components.
 * - `from_serial_date(int serial_date, std::string& date_str,
 *      const char* format) const`:
 *      Formats the serial date into a string according to the specified
 *      format rules for the new calendar.
 * - `from_serial_date(int serial_date, int& day_of_week) const`:
 *      Calculates the day of the week (usually based on the serial date
 *      modulo 7, but depends on the calendar's week definition if different).
 *
 * 3.  **Add to CalendarSystem Enum:**
 * In `calendar_system/CalendarSystem.hpp`, add a new identifier for your
 * calendar to the `enum CalendarSystem`.
 *
 * 4.  **Register in Date Class:**
 * - In `Date.cpp`, include the header for your new calendar system
 * (`NewCalendar.hpp`).
 * - Create a static instance of your new calendar class within the anonymous
 * namespace (similar to `gregorian_calendar`, `julian_calendar`, etc.).
 * - In the `Date::get_calendar_system` method, add a new `case` to the
 * `switch` statement that returns the static instance of your `NewCalendar`
 * class when the corresponding `CalendarSystem` enum value is passed.
 *
 * Remember to handle potential errors (e.g., invalid dates, out-of-range
 * serial dates for the specific calendar) by throwing appropriate
 * exceptions (like `std::out_of_range` or `std::invalid_argument`).
 */
#pragma once

#include <string>
#include <iostream>

#include <ex00/calendar_system/CalendarSystem.hpp>
#include <ex00/calendar_system/ICalendarSystem.hpp>

namespace toolbox {

class Date {
 public:
    Date();
    Date(const Date& other);
    Date& operator=(const Date& other);
    ~Date();

    static Date today();

    explicit Date(int serial_date);
    Date(CalendarSystem cal_sys, int era, int year, int month, int day);
    Date(CalendarSystem cal_sys, const std::string& date_str,
        const char* format = "%y-%m-%d", bool strict = true);

    std::string to_string(CalendarSystem cal_sys,
        const char* format = "%Y-%M-%D") const;

    int get_raw_date() const;
    int get_day(CalendarSystem cal_sys) const;
    int get_month(CalendarSystem cal_sys) const;
    int get_year(CalendarSystem cal_sys) const;
    int get_weekday(CalendarSystem cal_sys) const;  // 0=Sun, 1=Mon, ..., 6=Sat

    Date& operator++();  // pre-increment
    Date operator++(int);  // post-increment
    Date& operator--();  // pre-decrement
    Date operator--(int);  // post-decrement

    Date operator+(const int delta) const;
    Date operator-(const int delta) const;
    int operator-(const Date& other) const;
    Date& operator+=(const int delta);
    Date& operator-=(const int delta);

    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;

 private:
    void convert_from_serial_date(CalendarSystem cal_sys,
        int& era, int& year, int& month, int& day) const;
    void convert_from_serial_date(CalendarSystem cal_sys,
        std::string& date_str, const char* format) const;
    void convert_from_serial_date(CalendarSystem cal_sys,
        int& day_of_week) const;
    int convert_to_serial_date(CalendarSystem cal_sys,
        int era, int year, int month, int day) const;
    int convert_to_serial_date(CalendarSystem cal_sys,
        const std::string& date_str,
        const char* format, bool strict) const;
    ICalendarSystem& get_calendar_system(CalendarSystem cal_sys) const;

    int _serial_date;  // 0 mean 1970-01-01 (Unix epoch)
};

}  // namespace toolbox
