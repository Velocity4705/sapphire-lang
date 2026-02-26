#include "math.h"

using namespace sapphire::stdlib;

extern "C" {

double sapphire_math_sin(double x) {
    return Math::sin(x);
}

double sapphire_math_cos(double x) {
    return Math::cos(x);
}

double sapphire_math_tan(double x) {
    return Math::tan(x);
}

double sapphire_math_sqrt(double x) {
    return Math::sqrt(x);
}

double sapphire_math_pow(double x, double y) {
    return Math::pow(x, y);
}

double sapphire_math_abs(double x) {
    return Math::abs(x);
}

double sapphire_math_floor(double x) {
    return Math::floor(x);
}

double sapphire_math_ceil(double x) {
    return Math::ceil(x);
}

double sapphire_math_round(double x) {
    return Math::round(x);
}

} // extern "C"
