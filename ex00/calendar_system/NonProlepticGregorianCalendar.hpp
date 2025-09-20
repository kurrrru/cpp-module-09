// 1582-10-4の次の日が1582-10-15
#pragma once

#include <string>

#include <calendar_system/ICalendarSystem.hpp>

namespace toolbox {

class NonProlepticGregorianCalendar : public ICalendarSystem {
 public:
    NonProlepticGregorianCalendar();
    NonProlepticGregorianCalendar(const NonProlepticGregorianCalendar& other);
    NonProlepticGregorianCalendar& operator=(
        const NonProlepticGregorianCalendar& other);
    ~NonProlepticGregorianCalendar();

    int to_serial_date(int era, int year, int month, int day) const;
    int to_serial_date(const std::string& date_str,
        const char* format, bool strict) const;
    void from_serial_date(int serial_date,
        int& era, int& year, int& month, int& day) const;
    void from_serial_date(int serial_date,
        std::string& date_str, const char* format) const;
    void from_serial_date(int serial_date,
        int& day_of_week) const;  // 0=Sun, 1=Mon, ..., 6=Sat

    enum Era {
        BC,
        AD,
        END_OF_ERA
    };

 private:
    void validate_serial_date(int serial_date) const;
    void parse_formatted_date(const std::string& date_str,
        std::size_t pos,
        const char* format,
        int& era, bool era_found,
        int& year, bool year_found,
        int& month, bool month_found,
        int& day, bool day_found,
        bool& all_found,
        int& serial,
        bool strict) const;
    void parse_Ee(const std::string& date_str,
        std::size_t pos,
        const char* format,
        int& era, bool era_found,
        int& year, bool year_found,
        int& month, bool month_found,
        int& day, bool day_found,
        bool& all_found,
        int& serial,
        bool strict) const;
    void parse_Yy(const std::string& date_str,
        std::size_t pos,
        const char* format,
        int& era, bool era_found,
        int& year, bool year_found,
        int& month, bool month_found,
        int& day, bool day_found,
        bool& all_found,
        int& serial,
        bool strict) const;
    void parse_Mm(const std::string& date_str,
        std::size_t pos,
        const char* format,
        int& era, bool era_found,
        int& year, bool year_found,
        int& month, bool month_found,
        int& day, bool day_found,
        bool& all_found,
        int& serial,
        bool strict) const;
    void parse_Dd(const std::string& date_str,
        std::size_t pos,
        const char* format,
        int& era, bool era_found,
        int& year, bool year_found,
        int& month, bool month_found,
        int& day, bool day_found,
        bool& all_found,
        int& serial,
        bool strict) const;
};

}  // namespace toolbox
