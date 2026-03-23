#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <complex>

namespace Sapphire {
namespace MathX {

// ===== Number Theory =====
std::vector<int> sieve_of_eratosthenes(int limit);
bool is_prime(int64_t n);
std::vector<int64_t> prime_factors(int64_t n);
int64_t gcd(int64_t a, int64_t b);
int64_t lcm(int64_t a, int64_t b);
int64_t mod_pow(int64_t base, int64_t exp, int64_t mod);
int64_t mod_inverse(int64_t a, int64_t mod);
std::vector<int64_t> extended_gcd(int64_t a, int64_t b); // [g, x, y]
int64_t euler_totient(int64_t n);
bool is_perfect(int64_t n);
bool is_abundant(int64_t n);
std::vector<int64_t> divisors(int64_t n);
int64_t sum_divisors(int64_t n);
int64_t nth_prime(int n);
int64_t collatz_length(int64_t n);
int64_t digital_root(int64_t n);
bool is_palindrome_num(int64_t n);
int64_t reverse_num(int64_t n);
int64_t sum_digits(int64_t n);
bool is_pandigital(int64_t n, int digits);

// ===== Linear Algebra =====
// Matrix represented as flat vector, row-major, shape (rows x cols)
struct Matrix {
    std::vector<double> data;
    int rows, cols;
};

Matrix mat_create(int rows, int cols);
Matrix mat_identity(int n);
Matrix mat_from_vec(const std::vector<double>& data, int rows, int cols);
double mat_get(const Matrix& m, int r, int c);
void   mat_set(Matrix& m, int r, int c, double v);
Matrix mat_add(const Matrix& a, const Matrix& b);
Matrix mat_sub(const Matrix& a, const Matrix& b);
Matrix mat_mul(const Matrix& a, const Matrix& b);
Matrix mat_scale(const Matrix& m, double s);
Matrix mat_transpose(const Matrix& m);
double mat_det(const Matrix& m);
Matrix mat_inverse(const Matrix& m);
std::vector<double> mat_solve(const Matrix& A, const std::vector<double>& b); // Ax=b
double mat_trace(const Matrix& m);
std::string mat_to_string(const Matrix& m);

// Eigenvalues (power iteration for dominant eigenvalue)
double mat_dominant_eigenvalue(const Matrix& m, int max_iter = 1000);
std::vector<double> mat_dominant_eigenvector(const Matrix& m, int max_iter = 1000);

// SVD (simplified 2x2 and diagonal approximation)
struct SVDResult { Matrix U, S, Vt; };
SVDResult mat_svd(const Matrix& m);

// ===== Symbolic Math =====
struct Expr {
    std::string type;   // "num", "var", "add", "sub", "mul", "div", "pow", "neg", "sin", "cos", "exp", "ln"
    double value;       // for "num"
    std::string var;    // for "var"
    std::vector<Expr> children;
};

Expr sym_num(double v);
Expr sym_var(const std::string& name);
Expr sym_add(const Expr& a, const Expr& b);
Expr sym_sub(const Expr& a, const Expr& b);
Expr sym_mul(const Expr& a, const Expr& b);
Expr sym_div(const Expr& a, const Expr& b);
Expr sym_pow(const Expr& base, const Expr& exp);
Expr sym_neg(const Expr& a);
Expr sym_sin(const Expr& a);
Expr sym_cos(const Expr& a);
Expr sym_exp(const Expr& a);
Expr sym_ln(const Expr& a);

Expr sym_simplify(const Expr& e);
Expr sym_diff(const Expr& e, const std::string& var);
double sym_eval(const Expr& e, const std::string& var, double val);
std::string sym_to_string(const Expr& e);

// Numerical integration (Simpson's rule)
double integrate(double (*f)(double), double a, double b, int steps = 1000);
double integrate_expr(const Expr& e, const std::string& var, double a, double b, int steps = 1000);

// Root finding (Newton-Raphson)
double find_root(const Expr& f, const std::string& var, double x0, int max_iter = 100);

// ===== Statistics =====
double stat_mean(const std::vector<double>& v);
double stat_variance(const std::vector<double>& v);
double stat_stddev(const std::vector<double>& v);
double stat_median(std::vector<double> v);
double stat_correlation(const std::vector<double>& x, const std::vector<double>& y);
std::vector<double> stat_linear_regression(const std::vector<double>& x, const std::vector<double>& y); // [slope, intercept]

} // namespace MathX
} // namespace Sapphire

// ===== C API =====
extern "C" {

// ---- Number Theory ----
void*  mathx_sieve(int limit);
void   mathx_sieve_destroy(void* v);
int    mathx_sieve_count(void* v);
int    mathx_sieve_get(void* v, int i);
int    mathx_is_prime(long long n);
void*  mathx_prime_factors(long long n);
void   mathx_vec_destroy(void* v);
int    mathx_vec_count(void* v);
long long mathx_vec_get(void* v, int i);
long long mathx_gcd(long long a, long long b);
long long mathx_lcm(long long a, long long b);
long long mathx_mod_pow(long long base, long long exp, long long mod);
long long mathx_mod_inverse(long long a, long long mod);
long long mathx_euler_totient(long long n);
int    mathx_is_perfect(long long n);
int    mathx_is_abundant(long long n);
void*  mathx_divisors(long long n);
long long mathx_sum_divisors(long long n);
long long mathx_nth_prime(int n);
long long mathx_collatz_length(long long n);
long long mathx_digital_root(long long n);
int    mathx_is_palindrome_num(long long n);
long long mathx_reverse_num(long long n);
long long mathx_sum_digits(long long n);
int    mathx_is_pandigital(long long n, int digits);

// ---- Matrix ----
void*  mathx_mat_create(int rows, int cols);
void*  mathx_mat_identity(int n);
void   mathx_mat_destroy(void* m);
int    mathx_mat_rows(void* m);
int    mathx_mat_cols(void* m);
double mathx_mat_get(void* m, int r, int c);
void   mathx_mat_set(void* m, int r, int c, double v);
void*  mathx_mat_add(void* a, void* b);
void*  mathx_mat_sub(void* a, void* b);
void*  mathx_mat_mul(void* a, void* b);
void*  mathx_mat_scale(void* m, double s);
void*  mathx_mat_transpose(void* m);
double mathx_mat_det(void* m);
void*  mathx_mat_inverse(void* m);
void*  mathx_mat_solve(void* A, void* b_vec);
double mathx_mat_trace(void* m);
const char* mathx_mat_to_string(void* m);
double mathx_mat_dominant_eigenvalue(void* m, int max_iter);
void*  mathx_mat_dominant_eigenvector(void* m, int max_iter);

// ---- Symbolic ----
void*  mathx_sym_num(double v);
void*  mathx_sym_var(const char* name);
void*  mathx_sym_add(void* a, void* b);
void*  mathx_sym_sub(void* a, void* b);
void*  mathx_sym_mul(void* a, void* b);
void*  mathx_sym_div(void* a, void* b);
void*  mathx_sym_pow(void* base, void* exp);
void*  mathx_sym_neg(void* a);
void*  mathx_sym_sin(void* a);
void*  mathx_sym_cos(void* a);
void*  mathx_sym_exp(void* a);
void*  mathx_sym_ln(void* a);
void*  mathx_sym_simplify(void* e);
void*  mathx_sym_diff(void* e, const char* var);
double mathx_sym_eval(void* e, const char* var, double val);
const char* mathx_sym_to_string(void* e);
void   mathx_sym_destroy(void* e);
double mathx_integrate_expr(void* e, const char* var, double a, double b, int steps);
double mathx_find_root(void* f, const char* var, double x0, int max_iter);

// ---- Statistics ----
double mathx_stat_mean(void* vec);
double mathx_stat_variance(void* vec);
double mathx_stat_stddev(void* vec);
double mathx_stat_median(void* vec);
void*  mathx_dvec_create();
void   mathx_dvec_push(void* vec, double v);
void   mathx_dvec_destroy(void* vec);
int    mathx_dvec_count(void* vec);
double mathx_dvec_get(void* vec, int i);
double mathx_stat_correlation(void* x, void* y);
void*  mathx_stat_linear_regression(void* x, void* y); // returns dvec [slope, intercept]

} // extern "C"
