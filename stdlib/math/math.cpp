#include "math.h"

using namespace sapphire::stdlib;

extern "C" {

// Basic operations
double sapphire_math_abs(double x) {
    return Math::abs(x);
}

double sapphire_math_min(double a, double b) {
    return Math::min(a, b);
}

double sapphire_math_max(double a, double b) {
    return Math::max(a, b);
}

// Trigonometry
double sapphire_math_sin(double x) {
    return Math::sin(x);
}

double sapphire_math_cos(double x) {
    return Math::cos(x);
}

double sapphire_math_tan(double x) {
    return Math::tan(x);
}

double sapphire_math_asin(double x) {
    return Math::asin(x);
}

double sapphire_math_acos(double x) {
    return Math::acos(x);
}

double sapphire_math_atan(double x) {
    return Math::atan(x);
}

double sapphire_math_atan2(double y, double x) {
    return Math::atan2(y, x);
}

// Exponential and logarithmic
double sapphire_math_exp(double x) {
    return Math::exp(x);
}

double sapphire_math_log(double x) {
    return Math::log(x);
}

double sapphire_math_log10(double x) {
    return Math::log10(x);
}

double sapphire_math_log2(double x) {
    return Math::log2(x);
}

double sapphire_math_pow(double x, double y) {
    return Math::pow(x, y);
}

double sapphire_math_sqrt(double x) {
    return Math::sqrt(x);
}

// Rounding
double sapphire_math_floor(double x) {
    return Math::floor(x);
}

double sapphire_math_ceil(double x) {
    return Math::ceil(x);
}

double sapphire_math_round(double x) {
    return Math::round(x);
}

// Constants
double sapphire_math_pi() {
    return Math::PI;
}

double sapphire_math_e() {
    return Math::E;
}

} // extern "C"
