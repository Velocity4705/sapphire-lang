#ifndef SAPPHIRE_STDLIB_MATH_H
#define SAPPHIRE_STDLIB_MATH_H

#include <cmath>

namespace sapphire {
namespace stdlib {

/**
 * Math - Mathematical Functions
 * 
 * Standard mathematical operations.
 */
namespace Math {
    // Constants
    const double PI = 3.14159265358979323846;
    const double E = 2.71828182845904523536;
    const double TAU = 6.28318530717958647692;
    
    // Basic operations
    inline double abs(double x) { return std::fabs(x); }
    inline double min(double a, double b) { return a < b ? a : b; }
    inline double max(double a, double b) { return a > b ? a : b; }
    inline double clamp(double x, double min_val, double max_val) {
        return min(max(x, min_val), max_val);
    }
    
    // Trigonometry
    inline double sin(double x) { return std::sin(x); }
    inline double cos(double x) { return std::cos(x); }
    inline double tan(double x) { return std::tan(x); }
    inline double asin(double x) { return std::asin(x); }
    inline double acos(double x) { return std::acos(x); }
    inline double atan(double x) { return std::atan(x); }
    inline double atan2(double y, double x) { return std::atan2(y, x); }
    
    // Hyperbolic
    inline double sinh(double x) { return std::sinh(x); }
    inline double cosh(double x) { return std::cosh(x); }
    inline double tanh(double x) { return std::tanh(x); }
    
    // Exponential and logarithmic
    inline double exp(double x) { return std::exp(x); }
    inline double log(double x) { return std::log(x); }
    inline double log10(double x) { return std::log10(x); }
    inline double log2(double x) { return std::log2(x); }
    inline double pow(double x, double y) { return std::pow(x, y); }
    inline double sqrt(double x) { return std::sqrt(x); }
    inline double cbrt(double x) { return std::cbrt(x); }
    
    // Rounding
    inline double floor(double x) { return std::floor(x); }
    inline double ceil(double x) { return std::ceil(x); }
    inline double round(double x) { return std::round(x); }
    inline double trunc(double x) { return std::trunc(x); }
    
    // Other
    inline double fmod(double x, double y) { return std::fmod(x, y); }
    inline double remainder(double x, double y) { return std::remainder(x, y); }
    inline bool is_nan(double x) { return std::isnan(x); }
    inline bool is_inf(double x) { return std::isinf(x); }
    inline bool is_finite(double x) { return std::isfinite(x); }
    
    // Degrees/Radians conversion
    inline double to_radians(double degrees) { return degrees * PI / 180.0; }
    inline double to_degrees(double radians) { return radians * 180.0 / PI; }
}

} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    double sapphire_math_sin(double x);
    double sapphire_math_cos(double x);
    double sapphire_math_tan(double x);
    double sapphire_math_sqrt(double x);
    double sapphire_math_pow(double x, double y);
    double sapphire_math_abs(double x);
    double sapphire_math_floor(double x);
    double sapphire_math_ceil(double x);
    double sapphire_math_round(double x);
}

#endif // SAPPHIRE_STDLIB_MATH_H
