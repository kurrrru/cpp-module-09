#pragma once

#include <string>

namespace toolbox {

class ICalendarSystem {
 public:
    virtual ~ICalendarSystem() {}

    virtual int to_serial_date(int era,
        int year, int month, int day) const = 0;
    virtual int to_serial_date(const std::string& date_str,
        const char* format, bool strict = true) const = 0;
    virtual void from_serial_date(int serial_date,
        int& era, int& year, int& month, int& day) const = 0;
    virtual void from_serial_date(int serial_date,
        std::string& date_str, const char* format) const = 0;
    virtual void from_serial_date(int serial_date,
        int& day_of_week) const = 0;  // 0=Sun, 1=Mon, ..., 6=Sat
};

}  // namespace toolbox
