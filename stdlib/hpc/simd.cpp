#include "simd.h"
#include <cmath>
#include <thread>
#include <vector>
#include <algorithm>

namespace sapphire {
namespace stdlib {
namespace SIMD {

// Vec4f operations
Vec4f Vec4f::operator+(const Vec4f& other) const {
    Vec4f result;
    for (int i = 0; i < 4; i++) result.data[i] = data[i] + other.data[i];
    return result;
}

Vec4f Vec4f::operator-(const Vec4f& other) const {
    Vec4f result;
    for (int i = 0; i < 4; i++) result.data[i] = data[i] - other.data[i];
    return result;
}

Vec4f Vec4f::operator*(const Vec4f& other) const {
    Vec4f result;
    for (int i = 0; i < 4; i++) result.data[i] = data[i] * other.data[i];
    return result;
}

Vec4f Vec4f::operator/(const Vec4f& other) const {
    Vec4f result;
    for (int i = 0; i < 4; i++) result.data[i] = data[i] / other.data[i];
    return result;
}

float Vec4f::dot(const Vec4f& other) const {
    float sum = 0.0f;
    for (int i = 0; i < 4; i++) sum += data[i] * other.data[i];
    return sum;
}

float Vec4f::length() const {
    return std::sqrt(dot(*this));
}

Vec4f Vec4f::normalize() const {
    float len = length();
    Vec4f result;
    for (int i = 0; i < 4; i++) result.data[i] = data[i] / len;
    return result;
}

// Vectorized array operations
void add_arrays_f32(const float* a, const float* b, float* result, size_t size) {
    #ifdef __AVX__
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&result[i], vr);
    }
    for (; i < size; i++) {
        result[i] = a[i] + b[i];
    }
    #else
    for (size_t i = 0; i < size; i++) {
        result[i] = a[i] + b[i];
    }
    #endif
}

void mul_arrays_f32(const float* a, const float* b, float* result, size_t size) {
    #ifdef __AVX__
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vr = _mm256_mul_ps(va, vb);
        _mm256_storeu_ps(&result[i], vr);
    }
    for (; i < size; i++) {
        result[i] = a[i] * b[i];
    }
    #else
    for (size_t i = 0; i < size; i++) {
        result[i] = a[i] * b[i];
    }
    #endif
}

float dot_product_f32(const float* a, const float* b, size_t size) {
    float sum = 0.0f;
    #ifdef __AVX__
    __m256 vsum = _mm256_setzero_ps();
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        vsum = _mm256_add_ps(vsum, _mm256_mul_ps(va, vb));
    }
    float temp[8];
    _mm256_storeu_ps(temp, vsum);
    for (int j = 0; j < 8; j++) sum += temp[j];
    for (; i < size; i++) {
        sum += a[i] * b[i];
    }
    #else
    for (size_t i = 0; i < size; i++) {
        sum += a[i] * b[i];
    }
    #endif
    return sum;
}

void matrix_multiply_f32(const float* a, const float* b, float* result, int n) {
    // Cache-blocked matrix multiplication with SIMD
    const int block_size = 32;
    
    // Initialize result to zero
    for (int i = 0; i < n * n; i++) {
        result[i] = 0.0f;
    }
    
    // Blocked matrix multiplication
    for (int ii = 0; ii < n; ii += block_size) {
        for (int jj = 0; jj < n; jj += block_size) {
            for (int kk = 0; kk < n; kk += block_size) {
                // Process block
                int i_max = std::min(ii + block_size, n);
                int j_max = std::min(jj + block_size, n);
                int k_max = std::min(kk + block_size, n);
                
                for (int i = ii; i < i_max; i++) {
                    for (int j = jj; j < j_max; j++) {
                        float sum = result[i * n + j];
                        
                        #ifdef __AVX__
                        __m256 vsum = _mm256_setzero_ps();
                        int k = kk;
                        for (; k + 8 <= k_max; k += 8) {
                            __m256 va = _mm256_loadu_ps(&a[i * n + k]);
                            __m256 vb = _mm256_loadu_ps(&b[k * n + j]);
                            vsum = _mm256_add_ps(vsum, _mm256_mul_ps(va, vb));
                        }
                        float temp[8];
                        _mm256_storeu_ps(temp, vsum);
                        for (int t = 0; t < 8; t++) sum += temp[t];
                        
                        for (; k < k_max; k++) {
                            sum += a[i * n + k] * b[k * n + j];
                        }
                        #else
                        for (int k = kk; k < k_max; k++) {
                            sum += a[i * n + k] * b[k * n + j];
                        }
                        #endif
                        
                        result[i * n + j] = sum;
                    }
                }
            }
        }
    }
}

float sum_f32(const float* arr, size_t size) {
    float sum = 0.0f;
    #ifdef __AVX__
    __m256 vsum = _mm256_setzero_ps();
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 v = _mm256_loadu_ps(&arr[i]);
        vsum = _mm256_add_ps(vsum, v);
    }
    float temp[8];
    _mm256_storeu_ps(temp, vsum);
    for (int j = 0; j < 8; j++) sum += temp[j];
    for (; i < size; i++) {
        sum += arr[i];
    }
    #else
    for (size_t i = 0; i < size; i++) {
        sum += arr[i];
    }
    #endif
    return sum;
}

float max_f32(const float* arr, size_t size) {
    float max_val = arr[0];
    #ifdef __AVX__
    __m256 vmax = _mm256_set1_ps(arr[0]);
    size_t i = 1;
    for (; i + 8 <= size; i += 8) {
        __m256 v = _mm256_loadu_ps(&arr[i]);
        vmax = _mm256_max_ps(vmax, v);
    }
    float temp[8];
    _mm256_storeu_ps(temp, vmax);
    for (int j = 0; j < 8; j++) {
        if (temp[j] > max_val) max_val = temp[j];
    }
    for (; i < size; i++) {
        if (arr[i] > max_val) max_val = arr[i];
    }
    #else
    for (size_t i = 1; i < size; i++) {
        if (arr[i] > max_val) max_val = arr[i];
    }
    #endif
    return max_val;
}

float min_f32(const float* arr, size_t size) {
    float min_val = arr[0];
    #ifdef __AVX__
    __m256 vmin = _mm256_set1_ps(arr[0]);
    size_t i = 1;
    for (; i + 8 <= size; i += 8) {
        __m256 v = _mm256_loadu_ps(&arr[i]);
        vmin = _mm256_min_ps(vmin, v);
    }
    float temp[8];
    _mm256_storeu_ps(temp, vmin);
    for (int j = 0; j < 8; j++) {
        if (temp[j] < min_val) min_val = temp[j];
    }
    for (; i < size; i++) {
        if (arr[i] < min_val) min_val = arr[i];
    }
    #else
    for (size_t i = 1; i < size; i++) {
        if (arr[i] < min_val) min_val = arr[i];
    }
    #endif
    return min_val;
}

void parallel_add_f32(const float* a, const float* b, float* result, size_t size, int num_threads) {
    std::vector<std::thread> threads;
    size_t chunk_size = size / num_threads;
    
    for (int t = 0; t < num_threads; t++) {
        size_t start = t * chunk_size;
        size_t end = (t == num_threads - 1) ? size : start + chunk_size;
        
        threads.emplace_back([=]() {
            add_arrays_f32(&a[start], &b[start], &result[start], end - start);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

void parallel_mul_f32(const float* a, const float* b, float* result, size_t size, int num_threads) {
    std::vector<std::thread> threads;
    size_t chunk_size = size / num_threads;
    
    for (int t = 0; t < num_threads; t++) {
        size_t start = t * chunk_size;
        size_t end = (t == num_threads - 1) ? size : start + chunk_size;
        
        threads.emplace_back([=]() {
            mul_arrays_f32(&a[start], &b[start], &result[start], end - start);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

// Stub implementations for other functions
void add_arrays_f64(const double* a, const double* b, double* result, size_t size) {
    for (size_t i = 0; i < size; i++) result[i] = a[i] + b[i];
}

void mul_arrays_f64(const double* a, const double* b, double* result, size_t size) {
    for (size_t i = 0; i < size; i++) result[i] = a[i] * b[i];
}

double dot_product_f64(const double* a, const double* b, size_t size) {
    double sum = 0.0;
    for (size_t i = 0; i < size; i++) sum += a[i] * b[i];
    return sum;
}

Vec8f Vec8f::operator+(const Vec8f& other) const {
    Vec8f result;
    for (int i = 0; i < 8; i++) result.data[i] = data[i] + other.data[i];
    return result;
}

Vec8f Vec8f::operator-(const Vec8f& other) const {
    Vec8f result;
    for (int i = 0; i < 8; i++) result.data[i] = data[i] - other.data[i];
    return result;
}

Vec8f Vec8f::operator*(const Vec8f& other) const {
    Vec8f result;
    for (int i = 0; i < 8; i++) result.data[i] = data[i] * other.data[i];
    return result;
}

Vec8f Vec8f::operator/(const Vec8f& other) const {
    Vec8f result;
    for (int i = 0; i < 8; i++) result.data[i] = data[i] / other.data[i];
    return result;
}

} // namespace SIMD

namespace Memory {
    void* aligned_alloc(size_t alignment, size_t size) {
        return ::aligned_alloc(alignment, size);
    }
    
    void aligned_free(void* ptr) {
        free(ptr);
    }
    
    void prefetch(const void* addr) {
        __builtin_prefetch(addr, 0, 3);
    }
    
    void prefetch_write(void* addr) {
        __builtin_prefetch(addr, 1, 3);
    }
    
    void memory_fence() {
        __sync_synchronize();
    }
    
    void load_fence() {
        asm volatile("lfence" ::: "memory");
    }
    
    void store_fence() {
        asm volatile("sfence" ::: "memory");
    }
    
    void flush_cache_line(const void* addr) {
        asm volatile("clflush (%0)" :: "r"(addr) : "memory");
    }
    
    size_t get_cache_line_size() {
        return 64;  // Common cache line size
    }
}

namespace Parallel {
    // Template implementation moved to header
}

} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    void sapphire_simd_add_f32(const float* a, const float* b, float* result, size_t size) {
        sapphire::stdlib::SIMD::add_arrays_f32(a, b, result, size);
    }
    
    void sapphire_simd_mul_f32(const float* a, const float* b, float* result, size_t size) {
        sapphire::stdlib::SIMD::mul_arrays_f32(a, b, result, size);
    }
    
    float sapphire_simd_dot_f32(const float* a, const float* b, size_t size) {
        return sapphire::stdlib::SIMD::dot_product_f32(a, b, size);
    }
    
    void sapphire_simd_matmul_f32(const float* a, const float* b, float* result, int n) {
        sapphire::stdlib::SIMD::matrix_multiply_f32(a, b, result, n);
    }
    
    float sapphire_simd_sum_f32(const float* arr, size_t size) {
        return sapphire::stdlib::SIMD::sum_f32(arr, size);
    }
    
    void* sapphire_memory_aligned_alloc(size_t alignment, size_t size) {
        return sapphire::stdlib::Memory::aligned_alloc(alignment, size);
    }
    
    void sapphire_memory_aligned_free(void* ptr) {
        sapphire::stdlib::Memory::aligned_free(ptr);
    }
    
    void sapphire_memory_prefetch(const void* addr) {
        sapphire::stdlib::Memory::prefetch(addr);
    }
    
    void sapphire_memory_fence() {
        sapphire::stdlib::Memory::memory_fence();
    }
    
    void sapphire_parallel_add_f32(const float* a, const float* b, float* result, size_t size, int threads) {
        sapphire::stdlib::SIMD::parallel_add_f32(a, b, result, size, threads);
    }
}
