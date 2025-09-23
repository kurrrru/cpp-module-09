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
    void convert_form_serial_date(CalendarSystem cal_sys,
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
