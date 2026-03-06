#ifndef SAPPHIRE_STDLIB_SIMD_H
#define SAPPHIRE_STDLIB_SIMD_H

#include <cstdint>
#include <cstring>
#include <immintrin.h>  // AVX/SSE intrinsics

namespace sapphire {
namespace stdlib {

/**
 * SIMD - Single Instruction Multiple Data
 * 
 * High-performance vectorized operations to rival Mojo.
 * Provides zero-cost abstractions over CPU SIMD instructions.
 */

namespace SIMD {
    // Vector types
    struct Vec4f {
        float data[4];
        
        Vec4f() { memset(data, 0, sizeof(data)); }
        Vec4f(float x, float y, float z, float w) { data[0]=x; data[1]=y; data[2]=z; data[3]=w; }
        
        Vec4f operator+(const Vec4f& other) const;
        Vec4f operator-(const Vec4f& other) const;
        Vec4f operator*(const Vec4f& other) const;
        Vec4f operator/(const Vec4f& other) const;
        
        float dot(const Vec4f& other) const;
        float length() const;
        Vec4f normalize() const;
    };
    
    struct Vec8f {
        float data[8];
        
        Vec8f() { memset(data, 0, sizeof(data)); }
        
        Vec8f operator+(const Vec8f& other) const;
        Vec8f operator-(const Vec8f& other) const;
        Vec8f operator*(const Vec8f& other) const;
        Vec8f operator/(const Vec8f& other) const;
    };
    
    // Vectorized operations
    
    // Add arrays - O(n/4) with SSE, O(n/8) with AVX
    void add_arrays_f32(const float* a, const float* b, float* result, size_t size);
    void add_arrays_f64(const double* a, const double* b, double* result, size_t size);
    
    // Multiply arrays - O(n/4) with SSE, O(n/8) with AVX
    void mul_arrays_f32(const float* a, const float* b, float* result, size_t size);
    void mul_arrays_f64(const double* a, const double* b, double* result, size_t size);
    
    // Dot product - O(n/4) with SSE, O(n/8) with AVX
    float dot_product_f32(const float* a, const float* b, size_t size);
    double dot_product_f64(const double* a, const double* b, size_t size);
    
    // Matrix operations - O(n³/4) with SSE
    void matrix_multiply_f32(const float* a, const float* b, float* result, int n);
    
    // Reduction operations
    float sum_f32(const float* arr, size_t size);
    float max_f32(const float* arr, size_t size);
    float min_f32(const float* arr, size_t size);
    
    // Parallel operations (multi-threaded SIMD)
    void parallel_add_f32(const float* a, const float* b, float* result, size_t size, int threads);
    void parallel_mul_f32(const float* a, const float* b, float* result, size_t size, int threads);
}

// Memory Operations - Cache-optimized
namespace Memory {
    // Cache-aligned allocation
    void* aligned_alloc(size_t alignment, size_t size);
    void aligned_free(void* ptr);
    
    // Prefetching
    void prefetch(const void* addr);
    void prefetch_write(void* addr);
    
    // Memory barriers
    void memory_fence();
    void load_fence();
    void store_fence();
    
    // Cache control
    void flush_cache_line(const void* addr);
    size_t get_cache_line_size();
}

// Parallel Computing
namespace Parallel {
    // Parallel for loop
    void parallel_for(int start, int end, int threads, void (*func)(int));
    
    // Parallel reduce
    template<typename T>
    T parallel_reduce(const T* arr, size_t size, int threads, T (*op)(T, T));
    
    // Parallel map
    template<typename T>
    void parallel_map(const T* input, T* output, size_t size, int threads, T (*func)(T));
}

} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // SIMD operations
    void sapphire_simd_add_f32(const float* a, const float* b, float* result, size_t size);
    void sapphire_simd_mul_f32(const float* a, const float* b, float* result, size_t size);
    float sapphire_simd_dot_f32(const float* a, const float* b, size_t size);
    void sapphire_simd_matmul_f32(const float* a, const float* b, float* result, int n);
    float sapphire_simd_sum_f32(const float* arr, size_t size);
    
    // Memory operations
    void* sapphire_memory_aligned_alloc(size_t alignment, size_t size);
    void sapphire_memory_aligned_free(void* ptr);
    void sapphire_memory_prefetch(const void* addr);
    void sapphire_memory_fence();
    
    // Parallel operations
    void sapphire_parallel_add_f32(const float* a, const float* b, float* result, size_t size, int threads);
}

#endif // SAPPHIRE_STDLIB_SIMD_H
