#include "system.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <dirent.h>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <chrono>
#include <thread>

namespace sapphire {
namespace stdlib {
namespace System {

// Process Management
int get_pid() {
    return getpid();
}

int get_parent_pid() {
    return getppid();
}

int spawn_process(const std::string& command, const std::vector<std::string>& args) {
    pid_t pid = fork();
    if (pid == 0) {
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(command.c_str()));
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        execvp(command.c_str(), argv.data());
        exit(1);
    }
    return pid;
}

int wait_process(int pid) {
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
}

void exit_process(int code) {
    exit(code);
}

// Memory Management
void* allocate_memory(size_t size) {
    return malloc(size);
}

void free_memory(void* ptr) {
    free(ptr);
}

void* reallocate_memory(void* ptr, size_t new_size) {
    return realloc(ptr, new_size);
}

size_t get_page_size() {
    return sysconf(_SC_PAGESIZE);
}

size_t get_total_memory() {
    struct sysinfo info;
    sysinfo(&info);
    return info.totalram;
}

size_t get_available_memory() {
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram;
}

// File System
bool file_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool directory_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool create_directory(const std::string& path) {
    return mkdir(path.c_str(), 0755) == 0;
}

bool remove_file(const std::string& path) {
    return unlink(path.c_str()) == 0;
}

bool remove_directory(const std::string& path) {
    return rmdir(path.c_str()) == 0;
}

std::vector<std::string> list_directory(const std::string& path) {
    std::vector<std::string> entries;
    DIR* dir = opendir(path.c_str());
    if (!dir) return entries;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            entries.push_back(name);
        }
    }
    closedir(dir);
    return entries;
}

std::string get_current_directory() {
    char buffer[4096];
    if (getcwd(buffer, sizeof(buffer))) {
        return std::string(buffer);
    }
    return "";
}

bool change_directory(const std::string& path) {
    return chdir(path.c_str()) == 0;
}

// Environment
std::string get_env(const std::string& name) {
    const char* value = getenv(name.c_str());
    return value ? std::string(value) : "";
}

void set_env(const std::string& name, const std::string& value) {
    setenv(name.c_str(), value.c_str(), 1);
}

std::string get_username() {
    return get_env("USER");
}

std::string get_hostname() {
    char buffer[256];
    gethostname(buffer, sizeof(buffer));
    return std::string(buffer);
}

std::string get_os_name() {
    #ifdef __linux__
    return "Linux";
    #elif __APPLE__
    return "macOS";
    #elif _WIN32
    return "Windows";
    #else
    return "Unknown";
    #endif
}

std::string get_os_version() {
    return "1.0";
}

// System Info
int get_cpu_count() {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

std::string get_cpu_arch() {
    #ifdef __x86_64__
    return "x86_64";
    #elif __aarch64__
    return "aarch64";
    #elif __arm__
    return "arm";
    #else
    return "unknown";
    #endif
}

uint64_t get_uptime() {
    struct sysinfo info;
    sysinfo(&info);
    return info.uptime;
}

double get_load_average() {
    struct sysinfo info;
    sysinfo(&info);
    return info.loads[0] / 65536.0;
}

// Permissions
bool set_file_permissions(const std::string& path, int mode) {
    return chmod(path.c_str(), mode) == 0;
}

int get_file_permissions(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return buffer.st_mode & 0777;
    }
    return -1;
}

bool is_readable(const std::string& path) {
    return access(path.c_str(), R_OK) == 0;
}

bool is_writable(const std::string& path) {
    return access(path.c_str(), W_OK) == 0;
}

bool is_executable(const std::string& path) {
    return access(path.c_str(), X_OK) == 0;
}

// Signals
void send_signal(int pid, int signal) {
    kill(pid, signal);
}

void register_signal_handler(int signal, void (*handler)(int)) {
    std::signal(signal, handler);
}

// Time
uint64_t get_timestamp_ms() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

uint64_t get_timestamp_us() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

uint64_t get_timestamp_ns() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

void sleep_ms(uint64_t milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void sleep_us(uint64_t microseconds) {
    std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
}

// Networking
std::string get_ip_address() {
    return "127.0.0.1";
}

std::string get_mac_address() {
    return "00:00:00:00:00:00";
}

bool is_network_available() {
    return true;
}

} // namespace System

// Kernel Interface
namespace Kernel {
    int syscall(int number, void* arg1, void* arg2) {
        (void)number; (void)arg1; (void)arg2;
        return -1;
    }
    
    void register_interrupt_handler(int interrupt, void (*handler)()) {
        (void)interrupt; (void)handler;
    }
    
    void enable_interrupts() {}
    void disable_interrupts() {}
    
    void* map_memory(void* addr, size_t length, int prot, int flags) {
        (void)addr; (void)length; (void)prot; (void)flags;
        return nullptr;
    }
    
    void unmap_memory(void* addr, size_t length) {
        (void)addr; (void)length;
    }
    
    uint8_t inb(uint16_t port) {
        (void)port;
        return 0;
    }
    
    void outb(uint16_t port, uint8_t value) {
        (void)port; (void)value;
    }
    
    uint16_t inw(uint16_t port) {
        (void)port;
        return 0;
    }
    
    void outw(uint16_t port, uint16_t value) {
        (void)port; (void)value;
    }
    
    uint32_t inl(uint16_t port) {
        (void)port;
        return 0;
    }
    
    void outl(uint16_t port, uint32_t value) {
        (void)port; (void)value;
    }
}

} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    static thread_local char g_string_buffer[4096];
    
    int sapphire_system_get_pid() {
        return sapphire::stdlib::System::get_pid();
    }
    
    void sapphire_system_exit(int code) {
        sapphire::stdlib::System::exit_process(code);
    }
    
    void* sapphire_system_allocate(size_t size) {
        return sapphire::stdlib::System::allocate_memory(size);
    }
    
    void sapphire_system_free(void* ptr) {
        sapphire::stdlib::System::free_memory(ptr);
    }
    
    size_t sapphire_system_page_size() {
        return sapphire::stdlib::System::get_page_size();
    }
    
    bool sapphire_system_file_exists(const char* path) {
        return sapphire::stdlib::System::file_exists(path);
    }
    
    bool sapphire_system_create_directory(const char* path) {
        return sapphire::stdlib::System::create_directory(path);
    }
    
    const char* sapphire_system_get_cwd() {
        std::string cwd = sapphire::stdlib::System::get_current_directory();
        strncpy(g_string_buffer, cwd.c_str(), sizeof(g_string_buffer) - 1);
        return g_string_buffer;
    }
    
    const char* sapphire_system_get_env(const char* name) {
        std::string value = sapphire::stdlib::System::get_env(name);
        strncpy(g_string_buffer, value.c_str(), sizeof(g_string_buffer) - 1);
        return g_string_buffer;
    }
    
    const char* sapphire_system_get_username() {
        std::string username = sapphire::stdlib::System::get_username();
        strncpy(g_string_buffer, username.c_str(), sizeof(g_string_buffer) - 1);
        return g_string_buffer;
    }
    
    const char* sapphire_system_get_hostname() {
        std::string hostname = sapphire::stdlib::System::get_hostname();
        strncpy(g_string_buffer, hostname.c_str(), sizeof(g_string_buffer) - 1);
        return g_string_buffer;
    }
    
    int sapphire_system_cpu_count() {
        return sapphire::stdlib::System::get_cpu_count();
    }
    
    const char* sapphire_system_cpu_arch() {
        std::string arch = sapphire::stdlib::System::get_cpu_arch();
        strncpy(g_string_buffer, arch.c_str(), sizeof(g_string_buffer) - 1);
        return g_string_buffer;
    }
    
    uint64_t sapphire_system_timestamp_ms() {
        return sapphire::stdlib::System::get_timestamp_ms();
    }
    
    uint64_t sapphire_system_timestamp_us() {
        return sapphire::stdlib::System::get_timestamp_us();
    }
    
    uint64_t sapphire_system_timestamp_ns() {
        return sapphire::stdlib::System::get_timestamp_ns();
    }
    
    void sapphire_system_sleep_ms(uint64_t ms) {
        sapphire::stdlib::System::sleep_ms(ms);
    }
}
