#include <calendar_system/FrenchRepublicanCalendar.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstring>

#include <string.hpp>
#include <calendar_system/GregorianCalendar.hpp>

namespace {
bool is_leap(int year);
int last_day_of_month(int year, int month);

std::string get_month_name(int month);
std::string get_day_name(int month, int day);
std::string get_day_of_week_name(int day_of_week);

std::string to_string_Ee(int era, bool uppercase);
std::string to_string_Yy(int year, bool uppercase);
std::string to_string_Mm(int month, bool uppercase);
std::string to_string_Dd(int month, int day, bool uppercase);
std::string to_string_Ww(int day_of_week, bool uppercase);

toolbox::GregorianCalendar gregorian;

}  // namespace

namespace toolbox {
FrenchRepublicanCalendar::FrenchRepublicanCalendar() {
}

FrenchRepublicanCalendar::FrenchRepublicanCalendar(
    const FrenchRepublicanCalendar& other) {
    (void)other;
}

FrenchRepublicanCalendar& FrenchRepublicanCalendar::operator=(
    const FrenchRepublicanCalendar& other) {
    (void)other;
    return *this;
}

FrenchRepublicanCalendar::~FrenchRepublicanCalendar() {
}

int FrenchRepublicanCalendar::to_serial_date(int era,
        int year, int month, int day) const {
    const int start_year = 1;
    const int end_year = 14;
    if (era < 0 || era >= END_OF_ERA) {
        throw std::out_of_range(
            "FrenchRepublicanCalendar::to_serial_date failed: "
            "Invalid era: " + toolbox::to_string(era));
    }
    if (year < start_year || year > end_year) {
        throw std::out_of_range(
            "FrenchRepublicanCalendar::to_serial_date failed: "
            "year must be in " + toolbox::to_string(start_year)
            + ".." + toolbox::to_string(end_year));
    }
    if (month < 1 || month > 13) {
        throw std::out_of_range(
            "FrenchRepublicanCalendar::to_serial_date failed: "
            "month must be in 1..13");
    }
    if (day < 1 || day > last_day_of_month(year, month)) {
        throw std::out_of_range(
            "FrenchRepublicanCalendar::to_serial_date failed: "
            "day is out of range for month " + toolbox::to_string(month));
    }

    // The French Republican Calendar started on 22 September 1792 (Gregorian).
    const int start_serial = gregorian.to_serial_date(
        GregorianCalendar::AD, 1792, 9, 22);
    const int serial = start_serial
        + (year - 1) * 365
        + year / 4  // Leap year every 4 years (3, 7, 11)
        + (month - 1) * 30
        + (day - 1);
    return serial;
}

int FrenchRepublicanCalendar::to_serial_date(const std::string& date_str,
        const char* format, bool strict) const {
    if (!format) {
        throw std::invalid_argument(
            "FrenchRepublicanCalendar::to_serial_date failed: "
            "format is null");
    }
    int era = FrenchRepublicanCalendar::AD;
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
            "FrenchRepublicanCalendar::to_serial_date failed: "
            "date_str does not match format");
    }
    return serial;
}

void FrenchRepublicanCalendar::from_serial_date(int serial_date,
        int& era, int& year, int& month, int& day) const {
    const int start_serial = gregorian.to_serial_date(
        GregorianCalendar::AD, 1792, 9, 22);
    const int end_serial = gregorian.to_serial_date(
        GregorianCalendar::AD, 1806, 12, 31);
    if (serial_date < start_serial || serial_date > end_serial) {
        throw std::out_of_range(
            "FrenchRepublicanCalendar::from_serial_date failed: "
            "serial_date is out of range");
    }
    const int z = serial_date - start_serial;
    const int era_year = z / 1461;  // 4 years
    const int doe = z - era_year * 1461;
    const int yoe = (doe - doe / 1095) / 365;  // Leap year (3, 7, 11)
    year = era_year * 4 + yoe + 1;
    const int doy = doe - (yoe * 365 + (yoe + 1) / 4);
    month = doy / 30 + 1;
    day = doy % 30 + 1;
    era = FrenchRepublicanCalendar::AD;
}

void FrenchRepublicanCalendar::from_serial_date(int serial_date,
        std::string& date_str, const char* format) const {
    if (!format) {
        throw std::invalid_argument(
            "FrenchRepublicanCalendar::from_serial_date failed: "
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
                    ss << to_string_Dd(month, day, std::isupper(format[i]));
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
                        "FrenchRepublicanCalendar::from_serial_date failed: "
                        "Invalid format specifier: %"
                        + toolbox::to_string(format[i]));
            }
        } else {
            ss << format[i];
        }
    }
    date_str = ss.str();
}

void FrenchRepublicanCalendar::from_serial_date(int serial_date,
        int& day_of_week) const {
    int era, year, month, day;
    from_serial_date(serial_date, era, year, month, day);
    day_of_week = (day - 1) % 10;  // 10-day week
}

void FrenchRepublicanCalendar::parse_formatted_date(const std::string& date_str,
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
                    "FrenchRepublicanCalendar::parse_formatted_date failed: "
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
                    "FrenchRepublicanCalendar::parse_formatted_date failed: "
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
                    "FrenchRepublicanCalendar::parse_formatted_date failed: "
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
                    "FrenchRepublicanCalendar::parse_formatted_date failed: "
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
                    "FrenchRepublicanCalendar::parse_formatted_date failed: "
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
                "FrenchRepublicanCalendar::parse_formatted_date failed: "
                "Invalid format specifier: %" + toolbox::to_string(format[1]));
    }
}

void FrenchRepublicanCalendar::parse_Ee(const std::string& date_str,
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
        /* [toolbox::FrenchRepublicanCalendar::AD] = */ "A.D.",
    };
    const char* era_str_e[] = {
        /* [toolbox::FrenchRepublicanCalendar::AD] = */ "AD",
    };
    for (int i = 0; i < toolbox::FrenchRepublicanCalendar::END_OF_ERA; ++i) {
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
        "FrenchRepublicanCalendar::parse_Ee failed: "
        "era not found in date_str at position "
        + toolbox::to_string(pos));
}

void FrenchRepublicanCalendar::parse_Yy(const std::string& date_str,
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
    }
    return;
}

// get_month_nameを使う
void FrenchRepublicanCalendar::parse_Mm(const std::string& date_str,
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
    bool updated = false;
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
                int m = toolbox::stoi(month_str);
                if (!updated && !month_found && month != 0 && month != m) {
                    throw std::invalid_argument(
                        "FrenchRepublicanCalendar::parse_Mm failed: "
                        "month value conflict");
                }
                updated = (month != m);
                month = m;
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
        for (int m = 1; m <= 13; ++m) {
            std::string month_name = get_month_name(m);
            const std::size_t len = month_name.size();
            if (date_str.compare(pos, len, month_name) != 0) {
                continue;
            }
            if (!updated && !month_found && month != 0 && month != m) {
                continue;  // Value conflict
            }
            updated = (month != m);
            month = m;
            parse_formatted_date(date_str, pos + len, format + 2,
                era, era_found,
                year, year_found,
                month, true,
                day, day_found,
                all_found,
                serial,
                strict);
            return;
            if (all_found && !strict) {
                return;
            }
        }
    }
}

void FrenchRepublicanCalendar::parse_Dd(const std::string& date_str,
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
        for (int m = 1; m <= 13; ++m) {
            if (month_found && month != m) {
                continue;
            }
            const int last_day = last_day_of_month(3, m);  // leap year
            for (int d = 1; d <= last_day; ++d) {
                std::string day_name = get_day_name(m, d);
                const std::size_t len = day_name.size();
                if (date_str.compare(pos, len, day_name) != 0) {
                    continue;
                }
                day = d;
                month = m;
                parse_formatted_date(date_str, pos + len, format + 2,
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
        }
    }
}

}  // namespace toolbox

namespace {
bool is_leap(int year) {
    // Leap year rule is very complex, here we use a simplified rule
    // which is not historically accurate.
    return year % 4 == 3;  // Leap year every 4 years
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

// This implementation uses only ASCII characters.
// In a real implementation, accented characters should be used.
std::string get_month_name(int month) {
    const char* month_names[] = {
        /* 1  = */ "Vendemiaire",
        /* 2  = */ "Brumaire",
        /* 3  = */ "Frimaire",
        /* 4  = */ "Nivose",
        /* 5  = */ "Pluviose",
        /* 6  = */ "Ventose",
        /* 7  = */ "Germinal",
        /* 8  = */ "Floreal",
        /* 9  = */ "Prairial",
        /* 10 = */ "Messidor",
        /* 11 = */ "Thermidor",
        /* 12 = */ "Fructidor",
        /* 13 = */ "Sansculottides",
    };
    if (month < 1 || month > 13) {
        throw std::out_of_range("get_month_name failed: "
            "month must be in 1..13");
    }
    return month_names[month - 1];
}

std::string get_day_name(int month, int day) {
    if (month < 1 || month > 13) {
        throw std::out_of_range("get_day_name failed: "
            "month must be in 1..13");
    }
    if (day < 1 || (month == 13 && day > 6) || (month != 13 && day > 30)) {
        throw std::out_of_range("get_day_name failed: "
            "day is out of range for month " + toolbox::to_string(month));
    }
    int doy = (month - 1) * 30 + day;
    const char* day_names[] = {
        /* Vendemiaire (1) */
        "Raisin", "Safran", "Chataigne", "Colchique", "Cheval",
        "Balsamine", "Carotte", "Amaranthe", "Panais", "Cuve",
        "Pomme de terre", "Immortelle", "Potiron", "Reseda", "Ane",
        "Belle de nuit", "Citrouille", "Sarrasin", "Tournesol", "Pressoir",
        "Chanvre", "Peche", "Navet", "Amarillis", "Boeuf",
        "Aubergine", "Piment", "Tomate", "Orge", "Tonneau",

        /* Brumaire (2) */
        "Pomme", "Celeri", "Poire", "Betterave", "Oie",
        "Heliotrope", "Figue", "Scorsonere", "Alisier", "Charrue",
        "Salsifis", "Macre", "Topinambour", "Endive", "Dindon",
        "Chervis", "Cresson", "Dentelaire", "Grebade", "Herse",
        "Bacchante", "Azerole", "Garance", "Orange", "Faisan",
        "Pistache", "Mahjonc", "Coing", "Cormier", "Rouleau",

        /* Frimaire (3) */
        "Raiponce", "Turneps", "Chicoree", "Nefle", "Cochon",
        "Mache", "Chou-fleur", "Miel", "Genièvre", "Pioche",
        "Cire", "Raifort", "Cedre", "Sapin", "Chevreuil",
        "Ajonc", "Cypres", "Lierre", "Sabine", "Hoyau",
        "Erable sucre", "Bruyere", "Roseau", "Oseille", "Grillon",
        "Pignon", "Liege", "Truffe", "Olive", "Pelle",

        /* Nivose (4) */
        "Tourbe", "Houille", "Bitume", "Soufre", "Chien",
        "Lave", "Terre vegetale", "Fumier", "Salpetre", "Fleau",
        "Granit", "Argile", "Ardoise", "Gres", "Lapin",
        "Silex", "Marne", "Pierre a chaux", "Marbre", "Van",
        "Pierre a platre", "Sel", "Fer", "Cuivre", "Chat",
        "Etain", "Plomb", "Zinc", "Mercure", "Crible",

        /* Pluviose (5) */
        "Laureole", "Mousse", "Fragon", "Perce-neige", "Taureau",
        "Laurier-tin", "Amadouvier", "Mezereon", "Peuplier", "Coignee",
        "Ellebore", "Brocoli", "Laurier", "Avelinier", "Vache",
        "Buis", "Lichen", "If", "Pulmonaire", "Serpette",
        "Thlaspi", "Thimele", "Chiendent", "Trainel", "Lievre",
        "Guede", "Noisetier", "Cyclamen", "Chelidoine", "Traineau",

        /* Ventose (6) */
        "Tussilage", "Cornouiller", "Violier", "Troene", "Bouc",
        "Asaret", "Alaterne", "Violette", "Marceau", "Beche",
        "Narcisse", "Orme", "Fumeterre", "Velar", "Chevre",
        "Epinard", "Doronic", "Mouron", "Cerfeuil", "Cordeau",
        "Mandragore", "Persil", "Cochlearia", "Paquerette", "Thon",
        "Pissenlit", "Sylvie", "Capillaire", "Frene", "Plantoir",

        /* Germinal (7) */
        "Primevere", "Platane", "Asperge", "Tulipe", "Poule",
        "Bette", "Bouleau", "Jonquille", "Aulne", "Greffoir",
        "Pervenche", "Charme", "Morille", "Hetre", "Abeille",
        "Laitue", "Meleze", "Cigue", "Radis", "Ruche",
        "Gainier", "Romaine", "Marronnier", "Roquette", "Pigeon",
        "Lilas", "Anemone", "Pensee", "Myrtille", "Couvoir",

        /* Floreal (8) */
        "Rose", "Chene", "Fougere", "Aubepine", "Rossignol",
        "Ancolie", "Muguet", "Champignon", "Hyacinthe", "Rateau",
        "Rhubarbe", "Sainfoin", "Baton-d'or", "Chamerops", "Ver a soie",
        "Consoude", "Pimprenelle", "Corbeille d'or", "Arroche", "Sarcloir",
        "Statice", "Fritillaire", "Bourrache", "Valeriane", "Carpe",
        "Fusain", "Civette", "Buglosse", "Seneve", "Houlette",

        /* Prairial (9) */
        "Luzerne", "Hemerocalle", "Trefle", "Angelique", "Canard",
        "Melisse", "Fromental", "Martagon", "Serpolet", "Faux",
        "Fraise", "Betoine", "Pois", "Acacia", "Caille",
        "Oeillet", "Sureau", "Pavot", "Tilleul", "Fourche",
        "Barbeau", "Camomille", "Chevrefeuille", "Caille-lait", "Tanche",
        "Jasmin", "Verveine", "Thym", "Pivoine", "Chariot",

        /* Messidor (10) */
        "Seigle", "Avoine", "Oignon", "Veronique", "Mulet",
        "Romarin", "Concombre", "Echalote", "Absinthe", "Faucille",
        "Coriandre", "Artichaut", "Girofle", "Lavande", "Chamois",
        "Tabac", "Groseille", "Gesse", "Cerise", "Parc",
        "Menthe", "Cumin", "Haricot", "Orcanete", "Dindette",
        "Sauge", "Ail", "Vesce", "Ble", "Chalemie",

        /* Thermidor (11) */
        "Epeautre", "Bouillon-blanc", "Melon", "Ivraie", "Belier",
        "Prele", "Armoise", "Carthame", "Mure", "Arrosoir",
        "Panis", "Salicorne", "Abricot", "Basilic", "Brebis",
        "Guimauve", "Lin", "Amande", "Gentiane", "Ecluse",
        "Carline", "Caprier", "Lentille", "Aunee", "Loutre",
        "Myrte", "Colza", "Lupin", "Coton", "Moulin",

        /* Fructidor (12) */
        "Prune", "Millet", "Lycoperdon", "Escourgeon", "Saumon",
        "Tubereuse", "Sucrion", "Apocyn", "Reglisse", "Echelle",
        "Pasteque", "Fenouil", "Epine vinette", "Noix", "Truite",
        "Citron", "Cardere", "Nerprun", "Tagette", "Hotte",
        "Eglantier", "Noisette", "Houblon", "Sorgho", "Ecrevisse",
        "Bigarade", "Verge d'or", "Mais", "Marron", "Panier",

        /* Jours complementaires (13) */
        "Jour de la Vertu",
        "Jour du Genie",
        "Jour du Travail",
        "Jour de l'Opinion",
        "Jour des Recompenses",
        "Jour de la Revolution"
    };
    return day_names[doy - 1];
}

std::string get_day_of_week_name(int day_of_week) {
    const char* day_of_week_names[] = {
        /* 0 = */ "Primidi",
        /* 1 = */ "Duodi",
        /* 2 = */ "Tridi",
        /* 3 = */ "Quartidi",
        /* 4 = */ "Quintidi",
        /* 5 = */ "Sextidi",
        /* 6 = */ "Septidi",
        /* 7 = */ "Octidi",
        /* 8 = */ "Nonidi",
        /* 9 = */ "Decadi",
    };
    if (day_of_week < 0 || day_of_week > 9) {
        throw std::out_of_range("get_day_of_week_name failed: "
            "day_of_week must be in 0..9");
    }
    return day_of_week_names[day_of_week];
}

std::string to_string_Ee(int era, bool uppercase) {
    const char* era_str_E[] = {
        /* [toolbox::FrenchRepublicanCalendar::AD] = */ "A.D.",
    };
    const char* era_str_e[] = {
        /* [toolbox::FrenchRepublicanCalendar::AD] = */ "AD",
    };
    if (era < 0 || era >= toolbox::FrenchRepublicanCalendar::END_OF_ERA) {
        throw std::out_of_range(
            "to_string_Ee failed: Invalid era: " + toolbox::to_string(era));
    }
    return uppercase ? era_str_E[era] : era_str_e[era];
}

std::string to_string_Yy(int year, bool uppercase) {
    if (year < 1 || year > 14) {
        throw std::out_of_range("to_string_Yy failed: "
            "year must be in 1..14");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << year;
    } else {
        oss << std::setw(2) << std::setfill('0') << year;
    }
    return oss.str();
}

std::string to_string_Mm(int month, bool uppercase) {
    if (month < 1 || month > 13) {
        throw std::out_of_range("to_string_Mm failed: month must be in 1..13");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << month;
    } else {
        oss << get_month_name(month);
    }
    return oss.str();
}

std::string to_string_Dd(int month, int day, bool uppercase) {
    if (month < 1 || month > 13) {
        throw std::out_of_range("to_string_Dd failed: "
            "month must be in 1..13");
    }
    if (day < 1 || (month == 13 && day > 6) || (month != 13 && day > 30)) {
        throw std::out_of_range("to_string_Dd failed: "
            "day is out of range for month " + toolbox::to_string(month));
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << day;
    } else {
        oss << get_day_name(month, day);
    }
    return oss.str();
}

std::string to_string_Ww(int day_of_week, bool uppercase) {
    if (day_of_week < 0 || day_of_week > 9) {
        throw std::out_of_range("to_string_Ww failed: "
            "day_of_week must be in 0..9");
    }
    std::ostringstream oss;
    if (uppercase) {
        oss << day_of_week + 1;
    } else {
        oss << get_day_of_week_name(day_of_week);
    }
    return oss.str();
}

}  // namespace
