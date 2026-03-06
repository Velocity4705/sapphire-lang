#include "../system/system.h"
#include <iostream>
#include <cassert>

using namespace sapphire::stdlib;

int main() {
    std::cout << "Testing System library...\n\n";
    
    // Test Process
    std::cout << "Test 1: Process Info\n";
    int pid = System::get_pid();
    std::cout << "  PID: " << pid << "\n";
    assert(pid > 0);
    std::cout << "  ✓ Process test passed\n\n";
    
    // Test Memory
    std::cout << "Test 2: Memory Info\n";
    size_t page_size = System::get_page_size();
    size_t total_mem = System::get_total_memory();
    size_t free_mem = System::get_available_memory();
    std::cout << "  Page size: " << page_size << " bytes\n";
    std::cout << "  Total memory: " << total_mem / (1024*1024) << " MB\n";
    std::cout << "  Free memory: " << free_mem / (1024*1024) << " MB\n";
    assert(page_size > 0);
    std::cout << "  ✓ Memory test passed\n\n";
    
    // Test File System
    std::cout << "Test 3: File System\n";
    std::string cwd = System::get_current_directory();
    std::cout << "  CWD: " << cwd << "\n";
    assert(!cwd.empty());
    
    bool exists = System::file_exists("Makefile");
    std::cout << "  Makefile exists: " << (exists ? "yes" : "no") << "\n";
    assert(exists);
    std::cout << "  ✓ File system test passed\n\n";
    
    // Test Environment
    std::cout << "Test 4: Environment\n";
    std::string username = System::get_username();
    std::string hostname = System::get_hostname();
    std::string os = System::get_os_name();
    std::cout << "  User: " << username << "\n";
    std::cout << "  Host: " << hostname << "\n";
    std::cout << "  OS: " << os << "\n";
    assert(!username.empty());
    std::cout << "  ✓ Environment test passed\n\n";
    
    // Test System Info
    std::cout << "Test 5: System Info\n";
    int cpu_count = System::get_cpu_count();
    std::string arch = System::get_cpu_arch();
    std::cout << "  CPUs: " << cpu_count << "\n";
    std::cout << "  Arch: " << arch << "\n";
    assert(cpu_count > 0);
    std::cout << "  ✓ System info test passed\n\n";
    
    // Test High-Precision Timing
    std::cout << "Test 6: High-Precision Timing\n";
    uint64_t ts_ms = System::get_timestamp_ms();
    uint64_t ts_us = System::get_timestamp_us();
    uint64_t ts_ns = System::get_timestamp_ns();
    std::cout << "  Timestamp (ms): " << ts_ms << "\n";
    std::cout << "  Timestamp (us): " << ts_us << "\n";
    std::cout << "  Timestamp (ns): " << ts_ns << "\n";
    assert(ts_ns > ts_us);
    assert(ts_us > ts_ms);
    std::cout << "  ✓ Timing test passed\n\n";
    
    // Test C API
    std::cout << "Test 7: C API\n";
    int c_pid = sapphire_system_get_pid();
    assert(c_pid == pid);
    uint64_t c_ts = sapphire_system_timestamp_ms();
    assert(c_ts > 0);
    std::cout << "  ✓ C API test passed\n\n";
    
    std::cout << "All System tests passed! ✅\n";
    return 0;
}
