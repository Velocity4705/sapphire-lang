#ifndef SAPPHIRE_STDLIB_SYSTEM_H
#define SAPPHIRE_STDLIB_SYSTEM_H

#include <string>
#include <vector>
#include <cstdint>
#include <unistd.h>
#include <sys/types.h>

namespace sapphire {
namespace stdlib {

/**
 * System - Operating System Interface
 * 
 * Low-level system operations for OS development and systems programming.
 */

namespace System {
    // Process Management
    int get_pid();
    int get_parent_pid();
    int spawn_process(const std::string& command, const std::vector<std::string>& args);
    int wait_process(int pid);
    void exit_process(int code);
    
    // Memory Management
    void* allocate_memory(size_t size);
    void free_memory(void* ptr);
    void* reallocate_memory(void* ptr, size_t new_size);
    size_t get_page_size();
    size_t get_total_memory();
    size_t get_available_memory();
    
    // File System
    bool file_exists(const std::string& path);
    bool directory_exists(const std::string& path);
    bool create_directory(const std::string& path);
    bool remove_file(const std::string& path);
    bool remove_directory(const std::string& path);
    std::vector<std::string> list_directory(const std::string& path);
    std::string get_current_directory();
    bool change_directory(const std::string& path);
    
    // Environment
    std::string get_env(const std::string& name);
    void set_env(const std::string& name, const std::string& value);
    std::string get_username();
    std::string get_hostname();
    std::string get_os_name();
    std::string get_os_version();
    
    // System Info
    int get_cpu_count();
    std::string get_cpu_arch();
    uint64_t get_uptime();
    double get_load_average();
    
    // Permissions
    bool set_file_permissions(const std::string& path, int mode);
    int get_file_permissions(const std::string& path);
    bool is_readable(const std::string& path);
    bool is_writable(const std::string& path);
    bool is_executable(const std::string& path);
    
    // Signals
    void send_signal(int pid, int signal);
    void register_signal_handler(int signal, void (*handler)(int));
    
    // Time
    uint64_t get_timestamp_ms();
    uint64_t get_timestamp_us();
    uint64_t get_timestamp_ns();
    void sleep_ms(uint64_t milliseconds);
    void sleep_us(uint64_t microseconds);
    
    // Networking (basic)
    std::string get_ip_address();
    std::string get_mac_address();
    bool is_network_available();
}

// Kernel Interface (for OS development)
namespace Kernel {
    // System calls
    int syscall(int number, void* arg1 = nullptr, void* arg2 = nullptr);
    
    // Interrupt handling
    void register_interrupt_handler(int interrupt, void (*handler)());
    void enable_interrupts();
    void disable_interrupts();
    
    // Memory mapping
    void* map_memory(void* addr, size_t length, int prot, int flags);
    void unmap_memory(void* addr, size_t length);
    
    // Device I/O
    uint8_t inb(uint16_t port);
    void outb(uint16_t port, uint8_t value);
    uint16_t inw(uint16_t port);
    void outw(uint16_t port, uint16_t value);
    uint32_t inl(uint16_t port);
    void outl(uint16_t port, uint32_t value);
}

} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // Process
    int sapphire_system_get_pid();
    void sapphire_system_exit(int code);
    
    // Memory
    void* sapphire_system_allocate(size_t size);
    void sapphire_system_free(void* ptr);
    size_t sapphire_system_page_size();
    
    // File System
    bool sapphire_system_file_exists(const char* path);
    bool sapphire_system_create_directory(const char* path);
    const char* sapphire_system_get_cwd();
    
    // Environment
    const char* sapphire_system_get_env(const char* name);
    const char* sapphire_system_get_username();
    const char* sapphire_system_get_hostname();
    
    // System Info
    int sapphire_system_cpu_count();
    const char* sapphire_system_cpu_arch();
    
    // Time
    uint64_t sapphire_system_timestamp_ms();
    uint64_t sapphire_system_timestamp_us();
    uint64_t sapphire_system_timestamp_ns();
    void sapphire_system_sleep_ms(uint64_t ms);
}

#endif // SAPPHIRE_STDLIB_SYSTEM_H
