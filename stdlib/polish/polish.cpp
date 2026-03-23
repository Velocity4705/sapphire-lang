// polish.cpp - M17 Polish & Optimization for Sapphire
#include "polish.h"
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <numeric>

namespace Sapphire {
namespace Polish {

// ===== Profiler =====

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

struct ProfileData {
    uint64_t call_count = 0;
    double total_ms = 0;
    double min_ms = 1e18;
    double max_ms = 0;
    TimePoint start;
    bool running = false;
};

static std::map<std::string, ProfileData> g_profile_data;
static std::vector<std::string> g_profile_order;
static std::string g_profiler_report_buf;

void profiler_reset() {
    g_profile_data.clear();
    g_profile_order.clear();
}

void profiler_start(const std::string& name) {
    auto& d = g_profile_data[name];
    if (g_profile_data.count(name) == 1 && d.call_count == 0 && !d.running)
        g_profile_order.push_back(name);
    d.start = Clock::now();
    d.running = true;
}

void profiler_stop(const std::string& name) {
    auto it = g_profile_data.find(name);
    if (it == g_profile_data.end() || !it->second.running) return;
    auto& d = it->second;
    double ms = std::chrono::duration<double, std::milli>(Clock::now() - d.start).count();
    d.total_ms += ms;
    d.call_count++;
    d.min_ms = std::min(d.min_ms, ms);
    d.max_ms = std::max(d.max_ms, ms);
    d.running = false;
}

int profiler_entry_count() { return (int)g_profile_order.size(); }

ProfileEntry profiler_get(int i) {
    if (i < 0 || i >= (int)g_profile_order.size()) return {};
    const auto& name = g_profile_order[i];
    const auto& d = g_profile_data.at(name);
    ProfileEntry e;
    e.name = name;
    e.call_count = d.call_count;
    e.total_ms = d.total_ms;
    e.min_ms = d.call_count > 0 ? d.min_ms : 0;
    e.max_ms = d.max_ms;
    return e;
}

std::string profiler_report() {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "=== Profiler Report ===\n";
    oss << std::left << std::setw(24) << "Function"
        << std::right << std::setw(8) << "Calls"
        << std::setw(12) << "Total(ms)"
        << std::setw(12) << "Mean(ms)"
        << std::setw(12) << "Min(ms)"
        << std::setw(12) << "Max(ms)" << "\n";
    oss << std::string(80, '-') << "\n";
    for (const auto& name : g_profile_order) {
        const auto& d = g_profile_data.at(name);
        double mean = d.call_count > 0 ? d.total_ms / d.call_count : 0;
        oss << std::left << std::setw(24) << name
            << std::right << std::setw(8) << d.call_count
            << std::setw(12) << d.total_ms
            << std::setw(12) << mean
            << std::setw(12) << (d.call_count > 0 ? d.min_ms : 0)
            << std::setw(12) << d.max_ms << "\n";
    }
    return oss.str();
}

// ===== Benchmark =====

struct BenchData {
    std::string name;
    int iterations;
    int current_iter = 0;
    std::vector<double> times_ms;
    TimePoint iter_start;
    bool running = false;
    std::string report_buf;
};

void* bench_create(const std::string& name, int iterations) {
    auto* b = new BenchData();
    b->name = name;
    b->iterations = iterations;
    b->times_ms.reserve(iterations);
    return b;
}

void bench_destroy(void* b) { delete static_cast<BenchData*>(b); }

void bench_iter_start(void* b) {
    auto* bd = static_cast<BenchData*>(b);
    bd->iter_start = Clock::now();
    bd->running = true;
}

void bench_iter_stop(void* b) {
    auto* bd = static_cast<BenchData*>(b);
    if (!bd->running) return;
    double ms = std::chrono::duration<double, std::milli>(Clock::now() - bd->iter_start).count();
    bd->times_ms.push_back(ms);
    bd->current_iter++;
    bd->running = false;
}

BenchResult bench_result(void* b) {
    auto* bd = static_cast<BenchData*>(b);
    BenchResult r;
    r.name = bd->name;
    r.iterations = (int)bd->times_ms.size();
    if (bd->times_ms.empty()) return r;
    r.total_ms = std::accumulate(bd->times_ms.begin(), bd->times_ms.end(), 0.0);
    r.mean_ms = r.total_ms / r.iterations;
    r.min_ms = *std::min_element(bd->times_ms.begin(), bd->times_ms.end());
    r.max_ms = *std::max_element(bd->times_ms.begin(), bd->times_ms.end());
    r.ops_per_sec = r.mean_ms > 0 ? 1000.0 / r.mean_ms : 0;
    return r;
}

std::string bench_report(void* b) {
    auto r = bench_result(b);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "Benchmark: " << r.name << "\n"
        << "  Iterations : " << r.iterations << "\n"
        << "  Total (ms) : " << r.total_ms << "\n"
        << "  Mean  (ms) : " << r.mean_ms << "\n"
        << "  Min   (ms) : " << r.min_ms << "\n"
        << "  Max   (ms) : " << r.max_ms << "\n"
        << "  Ops/sec    : " << std::fixed << std::setprecision(1) << r.ops_per_sec << "\n";
    return oss.str();
}

// ===== Logger =====

struct LogMessage {
    int level;
    std::string tag;
    std::string msg;
    std::string formatted;
};

static int g_log_level = 2; // INFO
static bool g_log_timestamps = true;
static bool g_log_colors = true;
static std::vector<LogMessage> g_log_messages;
static std::ofstream g_log_file;
static std::string g_log_msg_buf;

static const char* level_name(int l) {
    switch(l) {
        case 0: return "TRACE";
        case 1: return "DEBUG";
        case 2: return "INFO ";
        case 3: return "WARN ";
        case 4: return "ERROR";
        case 5: return "FATAL";
        default: return "?????";
    }
}

static const char* level_color(int l) {
    switch(l) {
        case 0: return "\033[37m";   // white
        case 1: return "\033[36m";   // cyan
        case 2: return "\033[32m";   // green
        case 3: return "\033[33m";   // yellow
        case 4: return "\033[31m";   // red
        case 5: return "\033[35m";   // magenta
        default: return "\033[0m";
    }
}

void logger_set_level(int level) { g_log_level = level; }
int  logger_get_level() { return g_log_level; }

void logger_set_file(const std::string& path) {
    if (g_log_file.is_open()) g_log_file.close();
    g_log_file.open(path, std::ios::app);
}

void logger_close_file() {
    if (g_log_file.is_open()) g_log_file.close();
}

void logger_log(int level, const std::string& tag, const std::string& msg) {
    if (level < g_log_level) return;
    std::ostringstream oss;
    if (g_log_timestamps) {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count() % 1000;
        char buf[32];
        std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
        oss << "[" << buf << "." << std::setfill('0') << std::setw(3) << ms << "] ";
    }
    oss << "[" << level_name(level) << "]";
    if (!tag.empty()) oss << " [" << tag << "]";
    oss << " " << msg;
    std::string formatted = oss.str();

    LogMessage lm;
    lm.level = level;
    lm.tag = tag;
    lm.msg = msg;
    lm.formatted = formatted;
    g_log_messages.push_back(lm);

    // Output to stderr
    if (g_log_colors) {
        std::cerr << level_color(level) << formatted << "\033[0m\n";
    } else {
        std::cerr << formatted << "\n";
    }
    if (g_log_file.is_open()) {
        g_log_file << formatted << "\n";
        g_log_file.flush();
    }
}

void logger_enable_timestamps(bool on) { g_log_timestamps = on; }
void logger_enable_colors(bool on) { g_log_colors = on; }
int  logger_message_count() { return (int)g_log_messages.size(); }

std::string logger_get_message(int i) {
    if (i < 0 || i >= (int)g_log_messages.size()) return "";
    return g_log_messages[i].formatted;
}

void logger_clear() { g_log_messages.clear(); }

// ===== Stack Trace =====

struct StackTrace {
    std::vector<FrameInfo> frames;
    std::string str_buf;
};

void* stacktrace_capture() {
    auto* st = new StackTrace();
    // Simulate a meaningful stack trace (real unwinding requires platform libs)
    st->frames = {
        {"<current>",    "<script>", 0},
        {"visitCallExpr","interpreter.cpp", 0},
        {"execute",      "interpreter.cpp", 0},
        {"main",         "sapphire.cpp", 0},
    };
    return st;
}

void stacktrace_destroy(void* st) { delete static_cast<StackTrace*>(st); }

int stacktrace_depth(void* st) {
    return (int)static_cast<StackTrace*>(st)->frames.size();
}

FrameInfo stacktrace_frame(void* st, int i) {
    auto* s = static_cast<StackTrace*>(st);
    if (i < 0 || i >= (int)s->frames.size()) return {};
    return s->frames[i];
}

std::string stacktrace_to_string(void* st) {
    auto* s = static_cast<StackTrace*>(st);
    std::ostringstream oss;
    oss << "Stack trace (" << s->frames.size() << " frames):\n";
    for (int i = 0; i < (int)s->frames.size(); i++) {
        const auto& f = s->frames[i];
        oss << "  #" << i << "  " << f.function;
        if (!f.file.empty()) oss << " (" << f.file;
        if (f.line > 0) oss << ":" << f.line;
        if (!f.file.empty()) oss << ")";
        oss << "\n";
    }
    return oss.str();
}

// ===== Memory Tracker =====

struct MemTrack {
    size_t current = 0;
    size_t peak = 0;
    int alloc_count = 0;
    std::map<std::string, size_t> by_tag;
    std::string report_buf;
};

static MemTrack g_memtrack;

void memtrack_reset() { g_memtrack = MemTrack{}; }

void memtrack_alloc(const std::string& tag, size_t bytes) {
    g_memtrack.current += bytes;
    g_memtrack.peak = std::max(g_memtrack.peak, g_memtrack.current);
    g_memtrack.alloc_count++;
    g_memtrack.by_tag[tag] += bytes;
}

void memtrack_free(const std::string& tag, size_t bytes) {
    if (g_memtrack.current >= bytes) g_memtrack.current -= bytes;
    else g_memtrack.current = 0;
    if (g_memtrack.by_tag.count(tag)) {
        if (g_memtrack.by_tag[tag] >= bytes) g_memtrack.by_tag[tag] -= bytes;
        else g_memtrack.by_tag[tag] = 0;
    }
}

size_t memtrack_current_bytes() { return g_memtrack.current; }
size_t memtrack_peak_bytes()    { return g_memtrack.peak; }
int    memtrack_alloc_count()   { return g_memtrack.alloc_count; }

std::string memtrack_report() {
    std::ostringstream oss;
    oss << "=== Memory Tracker Report ===\n"
        << "  Current : " << g_memtrack.current << " bytes\n"
        << "  Peak    : " << g_memtrack.peak << " bytes\n"
        << "  Allocs  : " << g_memtrack.alloc_count << "\n"
        << "  By tag  :\n";
    for (const auto& [tag, bytes] : g_memtrack.by_tag)
        oss << "    " << tag << ": " << bytes << " bytes\n";
    return oss.str();
}

} // namespace Polish
} // namespace Sapphire
