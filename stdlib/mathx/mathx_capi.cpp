// mathx_capi.cpp - C API for M16 Mathematical Computing
#include "mathx.h"
#include <vector>
#include <string>
#include <cstring>

using namespace Sapphire::MathX;

static std::string g_mathx_last_string;

extern "C" {

// ===== Number Theory =====

struct IntVecWrapper { std::vector<int> v; };
struct LLVecWrapper  { std::vector<int64_t> v; };
struct DVecWrapper   { std::vector<double> v; };

void* mathx_sieve(int limit) {
    auto* w = new IntVecWrapper();
    w->v = sieve_of_eratosthenes(limit);
    return w;
}
void mathx_sieve_destroy(void* v) { delete static_cast<IntVecWrapper*>(v); }
int  mathx_sieve_count(void* v) { return (int)static_cast<IntVecWrapper*>(v)->v.size(); }
int  mathx_sieve_get(void* v, int i) { return static_cast<IntVecWrapper*>(v)->v[i]; }

int mathx_is_prime(long long n) { return is_prime((int64_t)n) ? 1 : 0; }

void* mathx_prime_factors(long long n) {
    auto* w = new LLVecWrapper();
    w->v = prime_factors((int64_t)n);
    return w;
}
void  mathx_vec_destroy(void* v) { delete static_cast<LLVecWrapper*>(v); }
int   mathx_vec_count(void* v) { return (int)static_cast<LLVecWrapper*>(v)->v.size(); }
long long mathx_vec_get(void* v, int i) { return (long long)static_cast<LLVecWrapper*>(v)->v[i]; }

long long mathx_gcd(long long a, long long b) { return (long long)gcd((int64_t)a, (int64_t)b); }
long long mathx_lcm(long long a, long long b) { return (long long)lcm((int64_t)a, (int64_t)b); }
long long mathx_mod_pow(long long base, long long exp, long long mod) { return (long long)mod_pow(base, exp, mod); }
long long mathx_mod_inverse(long long a, long long mod) { return (long long)mod_inverse(a, mod); }
long long mathx_euler_totient(long long n) { return (long long)euler_totient((int64_t)n); }
int  mathx_is_perfect(long long n) { return is_perfect((int64_t)n) ? 1 : 0; }
int  mathx_is_abundant(long long n) { return is_abundant((int64_t)n) ? 1 : 0; }

void* mathx_divisors(long long n) {
    auto* w = new LLVecWrapper();
    w->v = divisors((int64_t)n);
    return w;
}
long long mathx_sum_divisors(long long n) { return (long long)sum_divisors((int64_t)n); }
long long mathx_nth_prime(int n) { return (long long)nth_prime(n); }
long long mathx_collatz_length(long long n) { return (long long)collatz_length((int64_t)n); }
long long mathx_digital_root(long long n) { return (long long)digital_root((int64_t)n); }
int  mathx_is_palindrome_num(long long n) { return is_palindrome_num((int64_t)n) ? 1 : 0; }
long long mathx_reverse_num(long long n) { return (long long)reverse_num((int64_t)n); }
long long mathx_sum_digits(long long n) { return (long long)sum_digits((int64_t)n); }
int  mathx_is_pandigital(long long n, int digits) { return is_pandigital((int64_t)n, digits) ? 1 : 0; }

// ===== Matrix =====

void* mathx_mat_create(int rows, int cols) { return new Matrix(mat_create(rows, cols)); }
void* mathx_mat_identity(int n) { return new Matrix(mat_identity(n)); }
void  mathx_mat_destroy(void* m) { delete static_cast<Matrix*>(m); }
int   mathx_mat_rows(void* m) { return static_cast<Matrix*>(m)->rows; }
int   mathx_mat_cols(void* m) { return static_cast<Matrix*>(m)->cols; }
double mathx_mat_get(void* m, int r, int c) { return mat_get(*static_cast<Matrix*>(m), r, c); }
void  mathx_mat_set(void* m, int r, int c, double v) { mat_set(*static_cast<Matrix*>(m), r, c, v); }
void* mathx_mat_add(void* a, void* b) { return new Matrix(mat_add(*static_cast<Matrix*>(a), *static_cast<Matrix*>(b))); }
void* mathx_mat_sub(void* a, void* b) { return new Matrix(mat_sub(*static_cast<Matrix*>(a), *static_cast<Matrix*>(b))); }
void* mathx_mat_mul(void* a, void* b) { return new Matrix(mat_mul(*static_cast<Matrix*>(a), *static_cast<Matrix*>(b))); }
void* mathx_mat_scale(void* m, double s) { return new Matrix(mat_scale(*static_cast<Matrix*>(m), s)); }
void* mathx_mat_transpose(void* m) { return new Matrix(mat_transpose(*static_cast<Matrix*>(m))); }
double mathx_mat_det(void* m) { return mat_det(*static_cast<Matrix*>(m)); }
void* mathx_mat_inverse(void* m) { return new Matrix(mat_inverse(*static_cast<Matrix*>(m))); }
void* mathx_mat_solve(void* A, void* b_vec) {
    auto* bw = static_cast<DVecWrapper*>(b_vec);
    auto result = mat_solve(*static_cast<Matrix*>(A), bw->v);
    auto* w = new DVecWrapper(); w->v = result; return w;
}
double mathx_mat_trace(void* m) { return mat_trace(*static_cast<Matrix*>(m)); }
const char* mathx_mat_to_string(void* m) {
    g_mathx_last_string = mat_to_string(*static_cast<Matrix*>(m));
    return g_mathx_last_string.c_str();
}
double mathx_mat_dominant_eigenvalue(void* m, int max_iter) {
    return mat_dominant_eigenvalue(*static_cast<Matrix*>(m), max_iter);
}
void* mathx_mat_dominant_eigenvector(void* m, int max_iter) {
    auto* w = new DVecWrapper();
    w->v = mat_dominant_eigenvector(*static_cast<Matrix*>(m), max_iter);
    return w;
}

// ===== Symbolic =====

void* mathx_sym_num(double v) { return new Expr(sym_num(v)); }
void* mathx_sym_var(const char* name) { return new Expr(sym_var(name ? name : "x")); }
void* mathx_sym_add(void* a, void* b) { return new Expr(sym_add(*static_cast<Expr*>(a), *static_cast<Expr*>(b))); }
void* mathx_sym_sub(void* a, void* b) { return new Expr(sym_sub(*static_cast<Expr*>(a), *static_cast<Expr*>(b))); }
void* mathx_sym_mul(void* a, void* b) { return new Expr(sym_mul(*static_cast<Expr*>(a), *static_cast<Expr*>(b))); }
void* mathx_sym_div(void* a, void* b) { return new Expr(sym_div(*static_cast<Expr*>(a), *static_cast<Expr*>(b))); }
void* mathx_sym_pow(void* base, void* exp) { return new Expr(sym_pow(*static_cast<Expr*>(base), *static_cast<Expr*>(exp))); }
void* mathx_sym_neg(void* a) { return new Expr(sym_neg(*static_cast<Expr*>(a))); }
void* mathx_sym_sin(void* a) { return new Expr(sym_sin(*static_cast<Expr*>(a))); }
void* mathx_sym_cos(void* a) { return new Expr(sym_cos(*static_cast<Expr*>(a))); }
void* mathx_sym_exp(void* a) { return new Expr(sym_exp(*static_cast<Expr*>(a))); }
void* mathx_sym_ln(void* a)  { return new Expr(sym_ln(*static_cast<Expr*>(a))); }
void* mathx_sym_simplify(void* e) { return new Expr(sym_simplify(*static_cast<Expr*>(e))); }
void* mathx_sym_diff(void* e, const char* var) { return new Expr(sym_diff(*static_cast<Expr*>(e), var ? var : "x")); }
double mathx_sym_eval(void* e, const char* var, double val) { return sym_eval(*static_cast<Expr*>(e), var ? var : "x", val); }
const char* mathx_sym_to_string(void* e) {
    g_mathx_last_string = sym_to_string(*static_cast<Expr*>(e));
    return g_mathx_last_string.c_str();
}
void mathx_sym_destroy(void* e) { delete static_cast<Expr*>(e); }
double mathx_integrate_expr(void* e, const char* var, double a, double b, int steps) {
    return integrate_expr(*static_cast<Expr*>(e), var ? var : "x", a, b, steps > 0 ? steps : 1000);
}
double mathx_find_root(void* f, const char* var, double x0, int max_iter) {
    return find_root(*static_cast<Expr*>(f), var ? var : "x", x0, max_iter > 0 ? max_iter : 100);
}

// ===== Statistics =====

void* mathx_dvec_create() { return new DVecWrapper(); }
void  mathx_dvec_push(void* vec, double v) { static_cast<DVecWrapper*>(vec)->v.push_back(v); }
void  mathx_dvec_destroy(void* vec) { delete static_cast<DVecWrapper*>(vec); }
int   mathx_dvec_count(void* vec) { return (int)static_cast<DVecWrapper*>(vec)->v.size(); }
double mathx_dvec_get(void* vec, int i) { return static_cast<DVecWrapper*>(vec)->v[i]; }

double mathx_stat_mean(void* vec) { return stat_mean(static_cast<DVecWrapper*>(vec)->v); }
double mathx_stat_variance(void* vec) { return stat_variance(static_cast<DVecWrapper*>(vec)->v); }
double mathx_stat_stddev(void* vec) { return stat_stddev(static_cast<DVecWrapper*>(vec)->v); }
double mathx_stat_median(void* vec) { return stat_median(static_cast<DVecWrapper*>(vec)->v); }
double mathx_stat_correlation(void* x, void* y) {
    return stat_correlation(static_cast<DVecWrapper*>(x)->v, static_cast<DVecWrapper*>(y)->v);
}
void* mathx_stat_linear_regression(void* x, void* y) {
    auto* w = new DVecWrapper();
    w->v = stat_linear_regression(static_cast<DVecWrapper*>(x)->v, static_cast<DVecWrapper*>(y)->v);
    return w;
}

} // extern "C"
