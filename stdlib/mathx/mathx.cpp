// mathx.cpp - M16 Mathematical Computing for Sapphire
#include "mathx.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cassert>

namespace Sapphire {
namespace MathX {

// ===== Number Theory =====

std::vector<int> sieve_of_eratosthenes(int limit) {
    if (limit < 2) return {};
    std::vector<bool> is_p(limit + 1, true);
    is_p[0] = is_p[1] = false;
    for (int i = 2; i * i <= limit; i++)
        if (is_p[i])
            for (int j = i*i; j <= limit; j += i)
                is_p[j] = false;
    std::vector<int> primes;
    for (int i = 2; i <= limit; i++)
        if (is_p[i]) primes.push_back(i);
    return primes;
}

bool is_prime(int64_t n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int64_t i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

std::vector<int64_t> prime_factors(int64_t n) {
    std::vector<int64_t> factors;
    for (int64_t d = 2; d * d <= n; d++) {
        while (n % d == 0) { factors.push_back(d); n /= d; }
    }
    if (n > 1) factors.push_back(n);
    return factors;
}

int64_t gcd(int64_t a, int64_t b) {
    while (b) { a %= b; std::swap(a, b); }
    return a;
}

int64_t lcm(int64_t a, int64_t b) {
    return a / gcd(a, b) * b;
}

int64_t mod_pow(int64_t base, int64_t exp, int64_t mod) {
    int64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = (__int128)result * base % mod;
        base = (__int128)base * base % mod;
        exp >>= 1;
    }
    return result;
}

int64_t mod_inverse(int64_t a, int64_t mod) {
    return mod_pow(a, mod - 2, mod); // assumes mod is prime
}

std::vector<int64_t> extended_gcd(int64_t a, int64_t b) {
    if (b == 0) return {a, 1, 0};
    auto r = extended_gcd(b, a % b);
    return {r[0], r[2], r[1] - (a / b) * r[2]};
}

int64_t euler_totient(int64_t n) {
    int64_t result = n;
    for (int64_t p = 2; p * p <= n; p++) {
        if (n % p == 0) {
            while (n % p == 0) n /= p;
            result -= result / p;
        }
    }
    if (n > 1) result -= result / n;
    return result;
}

std::vector<int64_t> divisors(int64_t n) {
    std::vector<int64_t> divs;
    for (int64_t i = 1; i * i <= n; i++) {
        if (n % i == 0) {
            divs.push_back(i);
            if (i != n / i) divs.push_back(n / i);
        }
    }
    std::sort(divs.begin(), divs.end());
    return divs;
}

int64_t sum_divisors(int64_t n) {
    auto d = divisors(n);
    return std::accumulate(d.begin(), d.end(), (int64_t)0);
}

bool is_perfect(int64_t n) { return n > 1 && sum_divisors(n) - n == n; }
bool is_abundant(int64_t n) { return sum_divisors(n) - n > n; }

int64_t nth_prime(int n) {
    if (n <= 0) return -1;
    int count = 0;
    for (int64_t i = 2; ; i++)
        if (is_prime(i) && ++count == n) return i;
}

int64_t collatz_length(int64_t n) {
    int64_t len = 1;
    while (n != 1) {
        n = (n % 2 == 0) ? n / 2 : 3 * n + 1;
        len++;
    }
    return len;
}

int64_t digital_root(int64_t n) {
    if (n == 0) return 0;
    return 1 + (n - 1) % 9;
}

bool is_palindrome_num(int64_t n) {
    std::string s = std::to_string(n);
    std::string r = s; std::reverse(r.begin(), r.end());
    return s == r;
}

int64_t reverse_num(int64_t n) {
    std::string s = std::to_string(n);
    std::reverse(s.begin(), s.end());
    return std::stoll(s);
}

int64_t sum_digits(int64_t n) {
    int64_t s = 0;
    while (n > 0) { s += n % 10; n /= 10; }
    return s;
}

bool is_pandigital(int64_t n, int digits) {
    std::string s = std::to_string(n);
    if ((int)s.size() != digits) return false;
    std::vector<bool> seen(digits + 1, false);
    for (char c : s) {
        int d = c - '0';
        if (d < 1 || d > digits || seen[d]) return false;
        seen[d] = true;
    }
    return true;
}

// ===== Linear Algebra =====

Matrix mat_create(int rows, int cols) {
    Matrix m; m.rows = rows; m.cols = cols;
    m.data.assign(rows * cols, 0.0);
    return m;
}

Matrix mat_identity(int n) {
    Matrix m = mat_create(n, n);
    for (int i = 0; i < n; i++) m.data[i * n + i] = 1.0;
    return m;
}

Matrix mat_from_vec(const std::vector<double>& data, int rows, int cols) {
    Matrix m; m.rows = rows; m.cols = cols; m.data = data;
    return m;
}

double mat_get(const Matrix& m, int r, int c) { return m.data[r * m.cols + c]; }
void   mat_set(Matrix& m, int r, int c, double v) { m.data[r * m.cols + c] = v; }

Matrix mat_add(const Matrix& a, const Matrix& b) {
    Matrix r = mat_create(a.rows, a.cols);
    for (int i = 0; i < (int)a.data.size(); i++) r.data[i] = a.data[i] + b.data[i];
    return r;
}

Matrix mat_sub(const Matrix& a, const Matrix& b) {
    Matrix r = mat_create(a.rows, a.cols);
    for (int i = 0; i < (int)a.data.size(); i++) r.data[i] = a.data[i] - b.data[i];
    return r;
}

Matrix mat_mul(const Matrix& a, const Matrix& b) {
    Matrix r = mat_create(a.rows, b.cols);
    for (int i = 0; i < a.rows; i++)
        for (int k = 0; k < a.cols; k++)
            for (int j = 0; j < b.cols; j++)
                r.data[i * b.cols + j] += a.data[i * a.cols + k] * b.data[k * b.cols + j];
    return r;
}

Matrix mat_scale(const Matrix& m, double s) {
    Matrix r = m;
    for (auto& v : r.data) v *= s;
    return r;
}

Matrix mat_transpose(const Matrix& m) {
    Matrix r = mat_create(m.cols, m.rows);
    for (int i = 0; i < m.rows; i++)
        for (int j = 0; j < m.cols; j++)
            r.data[j * m.rows + i] = m.data[i * m.cols + j];
    return r;
}

double mat_det(const Matrix& m) {
    int n = m.rows;
    if (n == 1) return m.data[0];
    if (n == 2) return m.data[0]*m.data[3] - m.data[1]*m.data[2];
    // LU decomposition
    std::vector<double> a = m.data;
    double det = 1.0;
    for (int col = 0; col < n; col++) {
        int pivot = col;
        for (int row = col+1; row < n; row++)
            if (std::abs(a[row*n+col]) > std::abs(a[pivot*n+col])) pivot = row;
        if (pivot != col) {
            for (int j = 0; j < n; j++) std::swap(a[col*n+j], a[pivot*n+j]);
            det = -det;
        }
        if (std::abs(a[col*n+col]) < 1e-12) return 0.0;
        det *= a[col*n+col];
        for (int row = col+1; row < n; row++) {
            double f = a[row*n+col] / a[col*n+col];
            for (int j = col; j < n; j++) a[row*n+j] -= f * a[col*n+j];
        }
    }
    return det;
}

Matrix mat_inverse(const Matrix& m) {
    int n = m.rows;
    std::vector<double> a = m.data;
    Matrix inv = mat_identity(n);
    for (int col = 0; col < n; col++) {
        int pivot = col;
        for (int row = col+1; row < n; row++)
            if (std::abs(a[row*n+col]) > std::abs(a[pivot*n+col])) pivot = row;
        for (int j = 0; j < n; j++) {
            std::swap(a[col*n+j], a[pivot*n+j]);
            std::swap(inv.data[col*n+j], inv.data[pivot*n+j]);
        }
        double piv = a[col*n+col];
        for (int j = 0; j < n; j++) { a[col*n+j] /= piv; inv.data[col*n+j] /= piv; }
        for (int row = 0; row < n; row++) {
            if (row == col) continue;
            double f = a[row*n+col];
            for (int j = 0; j < n; j++) {
                a[row*n+j] -= f * a[col*n+j];
                inv.data[row*n+j] -= f * inv.data[col*n+j];
            }
        }
    }
    return inv;
}

std::vector<double> mat_solve(const Matrix& A, const std::vector<double>& b) {
    int n = A.rows;
    std::vector<double> a = A.data, rhs = b;
    for (int col = 0; col < n; col++) {
        int pivot = col;
        for (int row = col+1; row < n; row++)
            if (std::abs(a[row*n+col]) > std::abs(a[pivot*n+col])) pivot = row;
        std::swap(rhs[col], rhs[pivot]);
        for (int j = 0; j < n; j++) std::swap(a[col*n+j], a[pivot*n+j]);
        double piv = a[col*n+col];
        for (int j = 0; j < n; j++) a[col*n+j] /= piv;
        rhs[col] /= piv;
        for (int row = 0; row < n; row++) {
            if (row == col) continue;
            double f = a[row*n+col];
            for (int j = 0; j < n; j++) a[row*n+j] -= f * a[col*n+j];
            rhs[row] -= f * rhs[col];
        }
    }
    return rhs;
}

double mat_trace(const Matrix& m) {
    double t = 0;
    for (int i = 0; i < std::min(m.rows, m.cols); i++) t += m.data[i * m.cols + i];
    return t;
}

std::string mat_to_string(const Matrix& m) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    for (int i = 0; i < m.rows; i++) {
        oss << "[";
        for (int j = 0; j < m.cols; j++) {
            if (j) oss << ", ";
            oss << m.data[i * m.cols + j];
        }
        oss << "]\n";
    }
    return oss.str();
}

double mat_dominant_eigenvalue(const Matrix& m, int max_iter) {
    int n = m.rows;
    std::vector<double> v(n, 1.0);
    double eigenval = 0;
    for (int iter = 0; iter < max_iter; iter++) {
        std::vector<double> w(n, 0.0);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                w[i] += m.data[i * n + j] * v[j];
        double norm = 0;
        for (double x : w) norm = std::max(norm, std::abs(x));
        if (norm < 1e-12) break;
        eigenval = norm;
        for (double& x : w) x /= norm;
        v = w;
    }
    return eigenval;
}

std::vector<double> mat_dominant_eigenvector(const Matrix& m, int max_iter) {
    int n = m.rows;
    std::vector<double> v(n, 1.0);
    for (int iter = 0; iter < max_iter; iter++) {
        std::vector<double> w(n, 0.0);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                w[i] += m.data[i * n + j] * v[j];
        double norm = 0;
        for (double x : w) norm = std::max(norm, std::abs(x));
        if (norm < 1e-12) break;
        for (double& x : w) x /= norm;
        v = w;
    }
    return v;
}

SVDResult mat_svd(const Matrix& m) {
    // Simplified: compute A^T*A eigendecomposition for singular values
    Matrix At = mat_transpose(m);
    Matrix AtA = mat_mul(At, m);
    int n = AtA.rows;
    SVDResult r;
    r.S = mat_create(m.rows, m.cols);
    r.U = mat_identity(m.rows);
    r.Vt = mat_identity(m.cols);
    // Diagonal approximation: singular values = sqrt of diagonal of AtA
    for (int i = 0; i < std::min(m.rows, m.cols); i++)
        r.S.data[i * m.cols + i] = std::sqrt(std::max(0.0, AtA.data[i * n + i]));
    return r;
}

// ===== Symbolic Math =====

Expr sym_num(double v) { Expr e; e.type = "num"; e.value = v; return e; }
Expr sym_var(const std::string& name) { Expr e; e.type = "var"; e.var = name; return e; }

static Expr sym_binary(const std::string& op, const Expr& a, const Expr& b) {
    Expr e; e.type = op; e.children = {a, b}; return e;
}
static Expr sym_unary(const std::string& op, const Expr& a) {
    Expr e; e.type = op; e.children = {a}; return e;
}

Expr sym_add(const Expr& a, const Expr& b) { return sym_binary("add", a, b); }
Expr sym_sub(const Expr& a, const Expr& b) { return sym_binary("sub", a, b); }
Expr sym_mul(const Expr& a, const Expr& b) { return sym_binary("mul", a, b); }
Expr sym_div(const Expr& a, const Expr& b) { return sym_binary("div", a, b); }
Expr sym_pow(const Expr& base, const Expr& exp) { return sym_binary("pow", base, exp); }
Expr sym_neg(const Expr& a) { return sym_unary("neg", a); }
Expr sym_sin(const Expr& a) { return sym_unary("sin", a); }
Expr sym_cos(const Expr& a) { return sym_unary("cos", a); }
Expr sym_exp(const Expr& a) { return sym_unary("exp", a); }
Expr sym_ln(const Expr& a)  { return sym_unary("ln",  a); }

Expr sym_simplify(const Expr& e) {
    if (e.type == "num" || e.type == "var") return e;
    Expr a = e.children.size() > 0 ? sym_simplify(e.children[0]) : e;
    Expr b = e.children.size() > 1 ? sym_simplify(e.children[1]) : e;
    if (e.type == "add") {
        if (a.type == "num" && b.type == "num") return sym_num(a.value + b.value);
        if (a.type == "num" && a.value == 0) return b;
        if (b.type == "num" && b.value == 0) return a;
    } else if (e.type == "sub") {
        if (a.type == "num" && b.type == "num") return sym_num(a.value - b.value);
        if (b.type == "num" && b.value == 0) return a;
    } else if (e.type == "mul") {
        if (a.type == "num" && b.type == "num") return sym_num(a.value * b.value);
        if (a.type == "num" && a.value == 0) return sym_num(0);
        if (b.type == "num" && b.value == 0) return sym_num(0);
        if (a.type == "num" && a.value == 1) return b;
        if (b.type == "num" && b.value == 1) return a;
    } else if (e.type == "div") {
        if (a.type == "num" && b.type == "num" && b.value != 0) return sym_num(a.value / b.value);
        if (b.type == "num" && b.value == 1) return a;
    } else if (e.type == "pow") {
        if (a.type == "num" && b.type == "num") return sym_num(std::pow(a.value, b.value));
        if (b.type == "num" && b.value == 0) return sym_num(1);
        if (b.type == "num" && b.value == 1) return a;
    } else if (e.type == "neg") {
        if (a.type == "num") return sym_num(-a.value);
    }
    Expr r = e;
    r.children.clear();
    if (e.children.size() > 0) r.children.push_back(a);
    if (e.children.size() > 1) r.children.push_back(b);
    return r;
}

Expr sym_diff(const Expr& e, const std::string& var) {
    if (e.type == "num") return sym_num(0);
    if (e.type == "var") return sym_num(e.var == var ? 1 : 0);
    const Expr& u = e.children[0];
    const Expr& v = e.children.size() > 1 ? e.children[1] : e.children[0];
    if (e.type == "add") return sym_simplify(sym_add(sym_diff(u, var), sym_diff(v, var)));
    if (e.type == "sub") return sym_simplify(sym_sub(sym_diff(u, var), sym_diff(v, var)));
    if (e.type == "mul") return sym_simplify(sym_add(sym_mul(sym_diff(u, var), v), sym_mul(u, sym_diff(v, var))));
    if (e.type == "div") return sym_simplify(sym_div(sym_sub(sym_mul(sym_diff(u, var), v), sym_mul(u, sym_diff(v, var))), sym_mul(v, v)));
    if (e.type == "pow") {
        // d/dx x^n = n * x^(n-1)
        if (v.type == "num") return sym_simplify(sym_mul(sym_mul(v, sym_pow(u, sym_num(v.value - 1))), sym_diff(u, var)));
        return sym_num(0); // general case not handled
    }
    if (e.type == "neg") return sym_simplify(sym_neg(sym_diff(u, var)));
    if (e.type == "sin") return sym_simplify(sym_mul(sym_cos(u), sym_diff(u, var)));
    if (e.type == "cos") return sym_simplify(sym_neg(sym_mul(sym_sin(u), sym_diff(u, var))));
    if (e.type == "exp") return sym_simplify(sym_mul(sym_exp(u), sym_diff(u, var)));
    if (e.type == "ln")  return sym_simplify(sym_div(sym_diff(u, var), u));
    return sym_num(0);
}

double sym_eval(const Expr& e, const std::string& var, double val) {
    if (e.type == "num") return e.value;
    if (e.type == "var") return e.var == var ? val : 0.0;
    double a = e.children.size() > 0 ? sym_eval(e.children[0], var, val) : 0;
    double b = e.children.size() > 1 ? sym_eval(e.children[1], var, val) : 0;
    if (e.type == "add") return a + b;
    if (e.type == "sub") return a - b;
    if (e.type == "mul") return a * b;
    if (e.type == "div") return b != 0 ? a / b : 0;
    if (e.type == "pow") return std::pow(a, b);
    if (e.type == "neg") return -a;
    if (e.type == "sin") return std::sin(a);
    if (e.type == "cos") return std::cos(a);
    if (e.type == "exp") return std::exp(a);
    if (e.type == "ln")  return a > 0 ? std::log(a) : 0;
    return 0;
}

std::string sym_to_string(const Expr& e) {
    if (e.type == "num") {
        std::ostringstream oss;
        if (e.value == (int64_t)e.value) oss << (int64_t)e.value;
        else oss << e.value;
        return oss.str();
    }
    if (e.type == "var") return e.var;
    auto s0 = e.children.size() > 0 ? sym_to_string(e.children[0]) : "";
    auto s1 = e.children.size() > 1 ? sym_to_string(e.children[1]) : "";
    if (e.type == "add") return "(" + s0 + " + " + s1 + ")";
    if (e.type == "sub") return "(" + s0 + " - " + s1 + ")";
    if (e.type == "mul") return "(" + s0 + " * " + s1 + ")";
    if (e.type == "div") return "(" + s0 + " / " + s1 + ")";
    if (e.type == "pow") return "(" + s0 + "^" + s1 + ")";
    if (e.type == "neg") return "(-" + s0 + ")";
    if (e.type == "sin") return "sin(" + s0 + ")";
    if (e.type == "cos") return "cos(" + s0 + ")";
    if (e.type == "exp") return "exp(" + s0 + ")";
    if (e.type == "ln")  return "ln(" + s0 + ")";
    return "?";
}

double integrate_expr(const Expr& e, const std::string& var, double a, double b, int steps) {
    double h = (b - a) / steps;
    double sum = sym_eval(e, var, a) + sym_eval(e, var, b);
    for (int i = 1; i < steps; i++) {
        double x = a + i * h;
        sum += (i % 2 == 0 ? 2 : 4) * sym_eval(e, var, x);
    }
    return sum * h / 3.0;
}

double find_root(const Expr& f, const std::string& var, double x0, int max_iter) {
    Expr df = sym_diff(f, var);
    double x = x0;
    for (int i = 0; i < max_iter; i++) {
        double fx = sym_eval(f, var, x);
        double dfx = sym_eval(df, var, x);
        if (std::abs(dfx) < 1e-12) break;
        double xn = x - fx / dfx;
        if (std::abs(xn - x) < 1e-10) { x = xn; break; }
        x = xn;
    }
    return x;
}

// ===== Statistics =====

double stat_mean(const std::vector<double>& v) {
    if (v.empty()) return 0;
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

double stat_variance(const std::vector<double>& v) {
    if (v.size() < 2) return 0;
    double m = stat_mean(v);
    double s = 0;
    for (double x : v) s += (x - m) * (x - m);
    return s / (v.size() - 1);
}

double stat_stddev(const std::vector<double>& v) { return std::sqrt(stat_variance(v)); }

double stat_median(std::vector<double> v) {
    if (v.empty()) return 0;
    std::sort(v.begin(), v.end());
    int n = v.size();
    return n % 2 == 0 ? (v[n/2-1] + v[n/2]) / 2.0 : v[n/2];
}

double stat_correlation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.empty()) return 0;
    double mx = stat_mean(x), my = stat_mean(y);
    double num = 0, dx = 0, dy = 0;
    for (size_t i = 0; i < x.size(); i++) {
        num += (x[i] - mx) * (y[i] - my);
        dx  += (x[i] - mx) * (x[i] - mx);
        dy  += (y[i] - my) * (y[i] - my);
    }
    double denom = std::sqrt(dx * dy);
    return denom < 1e-12 ? 0 : num / denom;
}

std::vector<double> stat_linear_regression(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.empty()) return {0, 0};
    double mx = stat_mean(x), my = stat_mean(y);
    double num = 0, den = 0;
    for (size_t i = 0; i < x.size(); i++) {
        num += (x[i] - mx) * (y[i] - my);
        den += (x[i] - mx) * (x[i] - mx);
    }
    double slope = den < 1e-12 ? 0 : num / den;
    double intercept = my - slope * mx;
    return {slope, intercept};
}

} // namespace MathX
} // namespace Sapphire
