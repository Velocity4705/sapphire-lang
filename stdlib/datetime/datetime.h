#ifndef SAPPHIRE_STDLIB_DATETIME_H
#define SAPPHIRE_STDLIB_DATETIME_H

#include <ctime>
#include <chrono>
#include <string>
#include <cstdint>

namespace sapphire {
namespace stdlib {

/**
 * DateTime - Date and Time Utilities
 * 
 * Provides date, time, and duration functionality.
 */

class Duration {
public:
    int64_t milliseconds;
    
    Duration() : milliseconds(0) {}
    explicit Duration(int64_t ms) : milliseconds(ms) {}
    
    double seconds() const { return milliseconds / 1000.0; }
    double minutes() const { return milliseconds / 60000.0; }
    double hours() const { return milliseconds / 3600000.0; }
    int64_t total_milliseconds() const { return milliseconds; }
};

class Instant {
private:
    int64_t timestamp_ms;
    
public:
    Instant() : timestamp_ms(0) {}
    explicit Instant(int64_t ms) : timestamp_ms(ms) {}
    
    static Instant now();
    
    Duration operator-(const Instant& other) const {
        return Duration(timestamp_ms - other.timestamp_ms);
    }
    
    int64_t to_milliseconds() const { return timestamp_ms; }
};

class Date {
public:
    int year;
    int month;
    int day;
    
    Date() : year(1970), month(1), day(1) {}
    Date(int y, int m, int d) : year(y), month(m), day(d) {}
    
    static Date today();
    std::string to_string() const;
};

class Time {
public:
    int hour;
    int minute;
    int second;
    
    Time() : hour(0), minute(0), second(0) {}
    Time(int h, int m, int s) : hour(h), minute(m), second(s) {}
    
    static Time now();
    std::string to_string() const;
};

class DateTime {
public:
    Date date;
    Time time;
    
    DateTime() {}
    DateTime(const Date& d, const Time& t) : date(d), time(t) {}
    
    static DateTime now();
    std::string to_string() const;
};

} // namespace stdlib
} // namespace sapphire

// C API for code generation
extern "C" {
    int64_t sapphire_instant_now_ms();
    int64_t sapphire_duration_to_ms(int64_t duration);
    double sapphire_duration_to_seconds(int64_t duration);
    double sapphire_duration_to_minutes(int64_t duration);
    double sapphire_duration_to_hours(int64_t duration);
}

#endif // SAPPHIRE_STDLIB_DATETIME_H
