// polish_capi.cpp - C API for M17 Polish & Optimization
#include "polish.h"
#include <cstring>

using namespace Sapphire::Polish;

static std::string g_polish_str_buf;

extern "C" {

// ===== Profiler =====

void polish_profiler_reset() { profiler_reset(); }
void polish_profiler_start(const char* name) { profiler_start(name ? name : ""); }
void polish_profiler_stop(const char* name)  { profiler_stop(name ? name : ""); }
int  polish_profiler_entry_count() { return profiler_entry_count(); }

const char* polish_profiler_entry_name(int i) {
    g_polish_str_buf = profiler_get(i).name;
    return g_polish_str_buf.c_str();
}
long long polish_profiler_entry_calls(int i)    { return (long long)profiler_get(i).call_count; }
double    polish_profiler_entry_total_ms(int i) { return profiler_get(i).total_ms; }
double    polish_profiler_entry_mean_ms(int i) {
    auto e = profiler_get(i);
    return e.call_count > 0 ? e.total_ms / e.call_count : 0.0;
}
double    polish_profiler_entry_min_ms(int i)   { return profiler_get(i).min_ms; }
double    polish_profiler_entry_max_ms(int i)   { return profiler_get(i).max_ms; }
const char* polish_profiler_report() {
    g_polish_str_buf = profiler_report();
    return g_polish_str_buf.c_str();
}

// ===== Benchmark =====

void* polish_bench_create(const char* name, int iterations) {
    return bench_create(name ? name : "bench", iterations);
}
void  polish_bench_destroy(void* b)      { bench_destroy(b); }
void  polish_bench_iter_start(void* b)   { bench_iter_start(b); }
void  polish_bench_iter_stop(void* b)    { bench_iter_stop(b); }

int    polish_bench_iterations(void* b)  { return bench_result(b).iterations; }
double polish_bench_mean_ms(void* b)     { return bench_result(b).mean_ms; }
double polish_bench_min_ms(void* b)      { return bench_result(b).min_ms; }
double polish_bench_max_ms(void* b)      { return bench_result(b).max_ms; }
double polish_bench_ops_per_sec(void* b) { return bench_result(b).ops_per_sec; }

const char* polish_bench_report(void* b) {
    g_polish_str_buf = bench_report(b);
    return g_polish_str_buf.c_str();
}

// ===== Logger =====

void  polish_logger_set_level(int level)          { logger_set_level(level); }
int   polish_logger_get_level()                   { return logger_get_level(); }
void  polish_logger_set_file(const char* path)    { logger_set_file(path ? path : ""); }
void  polish_logger_close_file()                  { logger_close_file(); }
void  polish_logger_log(int level, const char* tag, const char* msg) {
    logger_log(level, tag ? tag : "", msg ? msg : "");
}
void  polish_logger_enable_timestamps(int on)     { logger_enable_timestamps(on != 0); }
void  polish_logger_enable_colors(int on)         { logger_enable_colors(on != 0); }
int   polish_logger_message_count()               { return logger_message_count(); }
const char* polish_logger_get_message(int i) {
    g_polish_str_buf = logger_get_message(i);
    return g_polish_str_buf.c_str();
}
void  polish_logger_clear() { logger_clear(); }

// ===== Stack Trace =====

void* polish_stacktrace_capture()              { return stacktrace_capture(); }
void  polish_stacktrace_destroy(void* st)      { stacktrace_destroy(st); }
int   polish_stacktrace_depth(void* st)        { return stacktrace_depth(st); }

const char* polish_stacktrace_frame_function(void* st, int i) {
    g_polish_str_buf = stacktrace_frame(st, i).function;
    return g_polish_str_buf.c_str();
}
const char* polish_stacktrace_frame_file(void* st, int i) {
    g_polish_str_buf = stacktrace_frame(st, i).file;
    return g_polish_str_buf.c_str();
}
int polish_stacktrace_frame_line(void* st, int i) {
    return stacktrace_frame(st, i).line;
}
const char* polish_stacktrace_to_string(void* st) {
    g_polish_str_buf = stacktrace_to_string(st);
    return g_polish_str_buf.c_str();
}

// ===== Memory Tracker =====

void      polish_memtrack_reset()                        { memtrack_reset(); }
void      polish_memtrack_alloc(const char* tag, long long bytes) {
    memtrack_alloc(tag ? tag : "", (size_t)bytes);
}
void      polish_memtrack_free(const char* tag, long long bytes) {
    memtrack_free(tag ? tag : "", (size_t)bytes);
}
long long polish_memtrack_current_bytes() { return (long long)memtrack_current_bytes(); }
long long polish_memtrack_peak_bytes()    { return (long long)memtrack_peak_bytes(); }
int       polish_memtrack_alloc_count()   { return memtrack_alloc_count(); }
const char* polish_memtrack_report() {
    g_polish_str_buf = memtrack_report();
    return g_polish_str_buf.c_str();
}

} // extern "C"
