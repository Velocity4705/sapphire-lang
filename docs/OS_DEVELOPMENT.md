# Sapphire OS Development Guide

**Status:** Production Ready ✅  
**Platform:** Linux (primary), Windows/macOS (partial)

## Overview

Sapphire provides comprehensive low-level system programming capabilities for operating system development, kernel modules, device drivers, and system utilities. This guide covers all OS development features.

---

## Table of Contents

1. [Memory Management](#memory-management)
2. [System Calls](#system-calls)
3. [Process Management](#process-management)
4. [Signal Handling](#signal-handling)
5. [Memory Mapping](#memory-mapping)
6. [Device I/O](#device-io)
7. [Interrupt Control](#interrupt-control)
8. [File System Operations](#file-system-operations)
9. [Permissions](#permissions)
10. [High-Precision Timing](#high-precision-timing)
11. [C API Reference](#c-api-reference)
12. [Examples](#examples)

---

## Memory Management

### Basic Memory Operations

```cpp
#include "stdlib/system/system.h"
using namespace sapphire::stdlib;

// Allocate memory
void* ptr = System::allocate_memory(1024);

// Reallocate
ptr = System::reallocate_memory(ptr, 2048);

// Free memory
System::free_memory(ptr);

// Get system memory info
size_t page_size = System::get_page_size();      // 4096 bytes
size_t total = System::get_total_memory();       // Total RAM
size_t available = System::get_available_memory(); // Free RAM
```

### Memory Mapping (mmap)

```cpp
#include <sys/mman.h>

// Map anonymous memory
void* addr = Kernel::map_memory(
    nullptr,                          // Let kernel choose address
    4096,                             // Size (one page)
    PROT_READ | PROT_WRITE,          // Read/write access
    MAP_PRIVATE | MAP_ANONYMOUS       // Private, not backed by file
);

// Use the memory
char* data = static_cast<char*>(addr);
strcpy(data, "Hello, mapped memory!");

// Unmap when done
Kernel::unmap_memory(addr, 4096);
```

### Memory Protection

```cpp
// Map read-only memory
void* ro_addr = Kernel::map_memory(
    nullptr, 4096,
    PROT_READ,                        // Read-only
    MAP_PRIVATE | MAP_ANONYMOUS
);

// Map executable memory (for JIT compilers)
void* exec_addr = Kernel::map_memory(
    nullptr, 4096,
    PROT_READ | PROT_WRITE | PROT_EXEC,  // RWX
    MAP_PRIVATE | MAP_ANONYMOUS
);
```

### Shared Memory

```cpp
// Create shared memory region
void* shared = Kernel::map_memory(
    nullptr, 4096,
    PROT_READ | PROT_WRITE,
    MAP_SHARED | MAP_ANONYMOUS        // Shared between processes
);

// Fork and share memory
int pid = fork();
if (pid == 0) {
    // Child can access shared memory
    char* data = static_cast<char*>(shared);
    strcpy(data, "From child");
    exit(0);
}
```

---

## System Calls

### Direct System Call Interface

```cpp
// Make direct system calls
int result = Kernel::syscall(
    39,        // System call number (SYS_getpid on x86_64)
    nullptr,   // Argument 1
    nullptr    // Argument 2
);

// Common system call numbers (x86_64 Linux):
// SYS_read = 0
// SYS_write = 1
// SYS_open = 2
// SYS_close = 3
// SYS_getpid = 39
// SYS_fork = 57
// SYS_execve = 59
```

### Example: Custom System Call

```cpp
// Read from file descriptor using syscall
char buffer[256];
ssize_t bytes = Kernel::syscall(
    0,                    // SYS_read
    (void*)0,            // fd = 0 (stdin)
    (void*)buffer        // buffer
);
```

---

## Process Management

### Process Information

```cpp
int pid = System::get_pid();           // Current process ID
int ppid = System::get_parent_pid();   // Parent process ID
```

### Spawning Processes

```cpp
// Spawn a new process
std::vector<std::string> args = {"-l", "/tmp"};
int child_pid = System::spawn_process("/bin/ls", args);

// Wait for process to complete
int exit_code = System::wait_process(child_pid);

// Exit current process
System::exit_process(0);
```

### Fork and Exec Pattern

```cpp
int pid = fork();
if (pid == 0) {
    // Child process
    std::vector<std::string> args = {"arg1", "arg2"};
    System::spawn_process("/path/to/program", args);
    System::exit_process(0);
} else {
    // Parent process
    int status = System::wait_process(pid);
}
```

---

## Signal Handling

### Registering Signal Handlers

```cpp
#include <signal.h>

// Define signal handler
void my_handler(int sig) {
    std::cout << "Received signal: " << sig << std::endl;
}

// Register handler
System::register_signal_handler(SIGUSR1, my_handler);
System::register_signal_handler(SIGINT, my_handler);
System::register_signal_handler(SIGTERM, my_handler);
```

### Sending Signals

```cpp
// Send signal to process
int target_pid = 1234;
System::send_signal(target_pid, SIGUSR1);

// Send signal to self
System::send_signal(System::get_pid(), SIGUSR1);

// Common signals:
// SIGINT  = 2  (Ctrl+C)
// SIGTERM = 15 (Termination)
// SIGUSR1 = 10 (User-defined)
// SIGUSR2 = 12 (User-defined)
// SIGKILL = 9  (Cannot be caught)
```

---

## Memory Mapping

### File-Backed Mapping

```cpp
#include <fcntl.h>

// Open file
int fd = open("/path/to/file", O_RDONLY);

// Map file into memory
void* addr = mmap(
    nullptr,
    file_size,
    PROT_READ,
    MAP_PRIVATE,
    fd,
    0  // offset
);

// Access file contents through memory
char* data = static_cast<char*>(addr);

// Unmap and close
munmap(addr, file_size);
close(fd);
```

### Memory-Mapped I/O (MMIO)

```cpp
// Map device memory (requires root)
void* device_mem = Kernel::map_memory(
    (void*)0xFEE00000,    // Physical address
    4096,
    PROT_READ | PROT_WRITE,
    MAP_SHARED            // Must be shared for MMIO
);

// Access device registers
volatile uint32_t* reg = static_cast<volatile uint32_t*>(device_mem);
*reg = 0x12345678;  // Write to device
```

---

## Device I/O

### Port I/O (x86/x86_64 only)

**Note:** Requires root privileges or `ioperm()`/`iopl()` system calls.

```cpp
// Read from I/O port
uint8_t value8 = Kernel::inb(0x3F8);    // Read byte
uint16_t value16 = Kernel::inw(0x3F8);  // Read word
uint32_t value32 = Kernel::inl(0x3F8);  // Read dword

// Write to I/O port
Kernel::outb(0x3F8, 0x41);              // Write byte
Kernel::outw(0x3F8, 0x4142);            // Write word
Kernel::outl(0x3F8, 0x41424344);        // Write dword
```

### Serial Port Example

```cpp
// Initialize serial port (COM1 = 0x3F8)
void init_serial() {
    Kernel::outb(0x3F8 + 1, 0x00);  // Disable interrupts
    Kernel::outb(0x3F8 + 3, 0x80);  // Enable DLAB
    Kernel::outb(0x3F8 + 0, 0x03);  // Set divisor (low byte)
    Kernel::outb(0x3F8 + 1, 0x00);  // Set divisor (high byte)
    Kernel::outb(0x3F8 + 3, 0x03);  // 8 bits, no parity, one stop bit
    Kernel::outb(0x3F8 + 2, 0xC7);  // Enable FIFO
    Kernel::outb(0x3F8 + 4, 0x0B);  // IRQs enabled, RTS/DSR set
}

// Write to serial port
void serial_write(char c) {
    while ((Kernel::inb(0x3F8 + 5) & 0x20) == 0);  // Wait for ready
    Kernel::outb(0x3F8, c);
}
```

---

## Interrupt Control

### Enable/Disable Interrupts (x86/x86_64)

**Note:** Requires kernel mode (ring 0). Will cause #GP fault in userspace.

```cpp
// Disable interrupts (CLI instruction)
Kernel::disable_interrupts();

// Critical section - interrupts disabled
// ... do atomic operations ...

// Enable interrupts (STI instruction)
Kernel::enable_interrupts();
```

### Interrupt Handler Registration

```cpp
// Register interrupt handler (userspace approximation)
void my_interrupt_handler() {
    // Handle interrupt
}

Kernel::register_interrupt_handler(14, my_interrupt_handler);  // Page fault
```

**Note:** True interrupt handling requires kernel module or OS development.

---

## File System Operations

### File Operations

```cpp
// Check if file exists
bool exists = System::file_exists("/path/to/file");

// Check if directory exists
bool dir_exists = System::directory_exists("/path/to/dir");

// Create directory
System::create_directory("/tmp/mydir");

// Remove file
System::remove_file("/tmp/file.txt");

// Remove directory
System::remove_directory("/tmp/mydir");

// List directory contents
std::vector<std::string> entries = System::list_directory("/tmp");
for (const auto& entry : entries) {
    std::cout << entry << std::endl;
}
```

### Directory Navigation

```cpp
// Get current directory
std::string cwd = System::get_current_directory();

// Change directory
System::change_directory("/tmp");
```

---

## Permissions

### File Permissions

```cpp
// Set file permissions (chmod)
System::set_file_permissions("/path/to/file", 0755);  // rwxr-xr-x

// Get file permissions
int mode = System::get_file_permissions("/path/to/file");

// Check access
bool readable = System::is_readable("/path/to/file");
bool writable = System::is_writable("/path/to/file");
bool executable = System::is_executable("/path/to/file");
```

---

## High-Precision Timing

### Timestamps

```cpp
// Get current timestamp
uint64_t ns = System::get_timestamp_ns();  // Nanoseconds
uint64_t us = System::get_timestamp_us();  // Microseconds
uint64_t ms = System::get_timestamp_ms();  // Milliseconds

// Sleep
System::sleep_ms(1000);  // Sleep 1 second
System::sleep_us(1000);  // Sleep 1 millisecond
```

### Benchmarking

```cpp
uint64_t start = System::get_timestamp_ns();

// ... code to benchmark ...

uint64_t end = System::get_timestamp_ns();
uint64_t elapsed = end - start;
std::cout << "Elapsed: " << elapsed << " ns" << std::endl;
```

---

## C API Reference

### Memory Management

```c
void* sapphire_system_allocate(size_t size);
void sapphire_system_free(void* ptr);
size_t sapphire_system_page_size();
```

### Kernel Interface

```c
int sapphire_kernel_syscall(int number, void* arg1, void* arg2);
void* sapphire_kernel_map_memory(void* addr, size_t length, int prot, int flags);
void sapphire_kernel_unmap_memory(void* addr, size_t length);
uint8_t sapphire_kernel_inb(uint16_t port);
void sapphire_kernel_outb(uint16_t port, uint8_t value);
uint16_t sapphire_kernel_inw(uint16_t port);
void sapphire_kernel_outw(uint16_t port, uint16_t value);
uint32_t sapphire_kernel_inl(uint16_t port);
void sapphire_kernel_outl(uint16_t port, uint32_t value);
void sapphire_kernel_enable_interrupts();
void sapphire_kernel_disable_interrupts();
```

### Process Management

```c
int sapphire_system_get_pid();
void sapphire_system_exit(int code);
```

### File System

```c
bool sapphire_system_file_exists(const char* path);
bool sapphire_system_create_directory(const char* path);
const char* sapphire_system_get_cwd();
```

### Environment

```c
const char* sapphire_system_get_env(const char* name);
const char* sapphire_system_get_username();
const char* sapphire_system_get_hostname();
```

### System Info

```c
int sapphire_system_cpu_count();
const char* sapphire_system_cpu_arch();
```

### Timing

```c
uint64_t sapphire_system_timestamp_ms();
uint64_t sapphire_system_timestamp_us();
uint64_t sapphire_system_timestamp_ns();
void sapphire_system_sleep_ms(uint64_t ms);
```

---

## Examples

### Example 1: Simple Kernel Module

```cpp
#include "stdlib/system/system.h"

// Kernel module entry point
extern "C" int init_module() {
    // Disable interrupts
    Kernel::disable_interrupts();
    
    // Register interrupt handler
    Kernel::register_interrupt_handler(14, page_fault_handler);
    
    // Enable interrupts
    Kernel::enable_interrupts();
    
    return 0;
}

// Kernel module exit point
extern "C" void cleanup_module() {
    // Cleanup
}
```

### Example 2: Device Driver

```cpp
// Simple character device driver
class CharDevice {
    uint16_t base_port;
    
public:
    CharDevice(uint16_t port) : base_port(port) {
        // Initialize device
        Kernel::outb(base_port, 0x00);
    }
    
    void write(char c) {
        // Wait for device ready
        while ((Kernel::inb(base_port + 5) & 0x20) == 0);
        // Write character
        Kernel::outb(base_port, c);
    }
    
    char read() {
        // Wait for data available
        while ((Kernel::inb(base_port + 5) & 0x01) == 0);
        // Read character
        return Kernel::inb(base_port);
    }
};
```

### Example 3: Memory-Mapped Device

```cpp
// Access memory-mapped device
class MMIODevice {
    volatile uint32_t* registers;
    
public:
    MMIODevice(uintptr_t phys_addr, size_t size) {
        registers = static_cast<volatile uint32_t*>(
            Kernel::map_memory(
                (void*)phys_addr,
                size,
                PROT_READ | PROT_WRITE,
                MAP_SHARED
            )
        );
    }
    
    void write_reg(int offset, uint32_t value) {
        registers[offset / 4] = value;
    }
    
    uint32_t read_reg(int offset) {
        return registers[offset / 4];
    }
};
```

---

## Platform Support

| Feature | Linux | Windows | macOS |
|---------|-------|---------|-------|
| Memory mapping | ✅ Full | ⚠️ Partial | ⚠️ Partial |
| System calls | ✅ Full | ❌ No | ❌ No |
| Port I/O | ✅ x86/x64 | ⚠️ Requires driver | ❌ No |
| Interrupts | ✅ Kernel mode | ❌ No | ❌ No |
| Process management | ✅ Full | ✅ Full | ✅ Full |
| Signal handling | ✅ Full | ⚠️ Limited | ✅ Full |
| File system | ✅ Full | ✅ Full | ✅ Full |

---

## Security Considerations

### Privilege Requirements

- **Port I/O:** Requires root or `ioperm()`/`iopl()`
- **Interrupt control:** Requires kernel mode (ring 0)
- **Memory mapping:** Some operations require root
- **System calls:** Available to all processes

### Best Practices

1. **Validate all inputs** from userspace
2. **Check return values** from system calls
3. **Use memory barriers** for concurrent access
4. **Handle signals** properly
5. **Clean up resources** (unmap memory, close files)

---

## Troubleshooting

### Common Issues

**Port I/O fails with EPERM:**
- Solution: Run with root privileges or use `ioperm()`

**Interrupt control causes segfault:**
- Solution: CLI/STI require kernel mode, use in kernel module

**Memory mapping fails:**
- Solution: Check permissions, alignment, and available memory

**System call returns -1:**
- Solution: Check `errno` for error code

---

## Conclusion

Sapphire provides comprehensive OS development capabilities:

✅ Direct system calls  
✅ Memory mapping (mmap/munmap)  
✅ Process management  
✅ Signal handling  
✅ Port I/O (x86/x86_64)  
✅ Interrupt control  
✅ File system operations  
✅ High-precision timing  

Use these features to build operating systems, kernel modules, device drivers, and system utilities.

**Status:** Production Ready for OS Development! 🔧✅
