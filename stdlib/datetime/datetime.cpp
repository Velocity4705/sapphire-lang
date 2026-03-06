#include "datetime.h"
#include <sstream>
#include <iomanip>

namespace sapphire {
namespace stdlib {

Instant Instant::now() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    return Instant(ms.count());
}

Date Date::today() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    return Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

std::string Date::to_string() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << year << "-"
        << std::setw(2) << month << "-"
        << std::setw(2) << day;
    return oss.str();
}

Time Time::now() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    return Time(tm.tm_hour, tm.tm_min, tm.tm_sec);
}

std::string Time::to_string() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hour << ":"
        << std::setw(2) << minute << ":"
        << std::setw(2) << second;
    return oss.str();
}

DateTime DateTime::now() {
    return DateTime(Date::today(), Time::now());
}

std::string DateTime::to_string() const {
    return date.to_string() + " " + time.to_string();
}

} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    int64_t sapphire_instant_now_ms() {
        return sapphire::stdlib::Instant::now().to_milliseconds();
    }
    
    int64_t sapphire_duration_to_ms(int64_t duration) {
        return duration;
    }
    
    double sapphire_duration_to_seconds(int64_t duration) {
        return duration / 1000.0;
    }
    
    double sapphire_duration_to_minutes(int64_t duration) {
        return duration / 60000.0;
    }
    
    double sapphire_duration_to_hours(int64_t duration) {
        return duration / 3600000.0;
    }
}
