#ifndef SAPPHIRE_STDLIB_FILE_H
#define SAPPHIRE_STDLIB_FILE_H

#include "../core/string.h"
#include "../core/vec.h"
#include <cstdio>
#include <string>

namespace sapphire {
namespace stdlib {

/**
 * File - File I/O
 * 
 * File operations for reading and writing.
 */
class File {
private:
    FILE* file_;
    String path_;
    String mode_;
    bool is_open_;
    
public:
    // Constructors
    File();
    ~File();
    
    // Delete copy operations (File is not copyable)
    File(const File&) = delete;
    File& operator=(const File&) = delete;
    
    // Move operations
    File(File&& other) noexcept;
    File& operator=(File&& other) noexcept;
    
    // Static factory
    static File open(const String& path, const String& mode);
    static File open(const char* path, const char* mode);
    
    // Open/Close
    bool open_file(const String& path, const String& mode);
    void close();
    bool is_open() const { return is_open_; }
    
    // Read operations
    String read();
    String read(size_t size);
    String read_line();
    Vec<String> read_lines();
    
    // Write operations
    void write(const String& data);
    void write_line(const String& line);
    void flush();
    
    // Properties
    const String& path() const { return path_; }
    const String& mode() const { return mode_; }
    
    // Static utilities
    static bool exists(const String& path);
    static bool remove_file(const String& path);
    static String read_all(const String& path);
    static void write_all(const String& path, const String& data);
};

} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    void* sapphire_file_open(const char* path, const char* mode);
    void sapphire_file_close(void* file);
    const char* sapphire_file_read(void* file);
    void sapphire_file_write(void* file, const char* data);
}

#endif // SAPPHIRE_STDLIB_FILE_H
