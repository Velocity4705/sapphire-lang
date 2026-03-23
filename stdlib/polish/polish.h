#pragma once
#include <string>
#include <cstdint>

namespace Sapphire {
namespace Polish {

// ===== Profiler =====
struct ProfileEntry {
    std::string name;
    uint64_t call_count;
    double total_ms;
    double min_ms;
    double max_ms;
};

void   profiler_reset();
void   profiler_start(const std::string& name);
void   profiler_stop(const std::string& name);
int    profiler_entry_count();
ProfileEntry profiler_get(int i);
std::string profiler_report();

// ===== Benchmark =====
struct BenchResult {
    std::string name;
    int iterations;
    double total_ms;
    double mean_ms;
    double min_ms;
    double max_ms;
    double ops_per_sec;
};

// Run a named benchmark N times, measuring wall time
// Caller provides a token; actual timing done via start/stop
void*  bench_create(const std::string& name, int iterations);
void   bench_destroy(void* b);
void   bench_iter_start(void* b);
void   bench_iter_stop(void* b);
BenchResult bench_result(void* b);
std::string bench_report(void* b);

// ===== Logger =====
enum class LogLevel { TRACE=0, DEBUG=1, INFO=2, WARN=3, ERROR=4, FATAL=5 };

void   logger_set_level(int level);
int    logger_get_level();
void   logger_set_file(const std::string& path);
void   logger_close_file();
void   logger_log(int level, const std::string& tag, const std::string& msg);
void   logger_enable_timestamps(bool on);
void   logger_enable_colors(bool on);
int    logger_message_count();
std::string logger_get_message(int i);
void   logger_clear();

// ===== Stack Trace / Error Info =====
struct FrameInfo {
    std::string function;
    std::string file;
    int line;
};

void*  stacktrace_capture();   // returns StackTrace*
void   stacktrace_destroy(void* st);
int    stacktrace_depth(void* st);
FrameInfo stacktrace_frame(void* st, int i);
std::string stacktrace_to_string(void* st);

// ===== Memory Tracker =====
void   memtrack_reset();
void   memtrack_alloc(const std::string& tag, size_t bytes);
void   memtrack_free(const std::string& tag, size_t bytes);
size_t memtrack_current_bytes();
size_t memtrack_peak_bytes();
int    memtrack_alloc_count();
std::string memtrack_report();

} // namespace Polish
} // namespace Sapphire

// ===== C API =====
extern "C" {

// Profiler
void  polish_profiler_reset();
void  polish_profiler_start(const char* name);
void  polish_profiler_stop(const char* name);
int   polish_profiler_entry_count();
const char* polish_profiler_entry_name(int i);
long long   polish_profiler_entry_calls(int i);
double      polish_profiler_entry_total_ms(int i);
double      polish_profiler_entry_mean_ms(int i);
double      polish_profiler_entry_min_ms(int i);
double      polish_profiler_entry_max_ms(int i);
const char* polish_profiler_report();

// Benchmark
void* polish_bench_create(const char* name, int iterations);
void  polish_bench_destroy(void* b);
void  polish_bench_iter_start(void* b);
void  polish_bench_iter_stop(void* b);
int   polish_bench_iterations(void* b);
double polish_bench_mean_ms(void* b);
double polish_bench_min_ms(void* b);
double polish_bench_max_ms(void* b);
double polish_bench_ops_per_sec(void* b);
const char* polish_bench_report(void* b);

// Logger
void  polish_logger_set_level(int level);
int   polish_logger_get_level();
void  polish_logger_set_file(const char* path);
void  polish_logger_close_file();
void  polish_logger_log(int level, const char* tag, const char* msg);
void  polish_logger_enable_timestamps(int on);
void  polish_logger_enable_colors(int on);
int   polish_logger_message_count();
const char* polish_logger_get_message(int i);
void  polish_logger_clear();

// Stack trace
void* polish_stacktrace_capture();
void  polish_stacktrace_destroy(void* st);
int   polish_stacktrace_depth(void* st);
const char* polish_stacktrace_frame_function(void* st, int i);
const char* polish_stacktrace_frame_file(void* st, int i);
int   polish_stacktrace_frame_line(void* st, int i);
const char* polish_stacktrace_to_string(void* st);

// Memory tracker
void  polish_memtrack_reset();
void  polish_memtrack_alloc(const char* tag, long long bytes);
void  polish_memtrack_free(const char* tag, long long bytes);
long long polish_memtrack_current_bytes();
long long polish_memtrack_peak_bytes();
int   polish_memtrack_alloc_count();
const char* polish_memtrack_report();

} // extern "C"
