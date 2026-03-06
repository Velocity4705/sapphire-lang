#ifndef SAPPHIRE_STDLIB_FILE_H
#define SAPPHIRE_STDLIB_FILE_H

#include "../core/string.h"
#include "../core/vec.h"
#include <cstdio>
#include <string>
#include <vector>
#include <cstdint>

namespace sapphire {
namespace stdlib {

/**
 * File - File I/O operations
 * 
 * Provides static methods for reading, writing, and manipulating files.
 */
class File {
public:
    // Read operations
    static std::string read(const std::string& path);
    static std::vector<std::string> read_lines(const std::string& path);
    static std::vector<uint8_t> read_bytes(const std::string& path);
    
    // Write operations
    static void write(const std::string& path, const std::string& content);
    static void write_lines(const std::string& path, const std::vector<std::string>& lines);
    static void write_bytes(const std::string& path, const std::vector<uint8_t>& bytes);
    static void append(const std::string& path, const std::string& content);
    
    // File operations
    static bool exists(const std::string& path);
    static void delete_file(const std::string& path);
    static void copy(const std::string& source, const std::string& destination);
    static void move(const std::string& source, const std::string& destination);
    static size_t size(const std::string& path);
};

/**
 * Dir - Directory operations
 * 
 * Provides static methods for working with directories.
 */
class Dir {
public:
    // Directory operations
    static std::vector<std::string> list(const std::string& path);
    static bool exists(const std::string& path);
    static void create(const std::string& path);
    static void create_all(const std::string& path);
    static void remove(const std::string& path);
    static void remove_all(const std::string& path);
};

/**
 * Path - Path manipulation utilities
 * 
 * Provides static methods for working with file paths.
 */
class Path {
public:
    // Path operations
    static std::string join(const std::string& part1, const std::string& part2);
    static std::string join(const std::string& part1, const std::string& part2, const std::string& part3);
    static std::string join(const std::vector<std::string>& parts);
    static std::string basename(const std::string& path);
    static std::string dirname(const std::string& path);
    static std::string extension(const std::string& path);
    static std::string absolute(const std::string& path);
    
private:
    static char separator();
};

} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // File operations
    const char* sapphire_file_read(const char* path);
    void sapphire_file_write(const char* path, const char* content);
    int sapphire_file_exists(const char* path);
    void sapphire_file_delete(const char* path);
    void sapphire_file_copy(const char* source, const char* destination);
    size_t sapphire_file_size(const char* path);
    
    // Directory operations
    int sapphire_dir_exists(const char* path);
    void sapphire_dir_create(const char* path);
    void sapphire_dir_remove(const char* path);
    
    // Path operations
    const char* sapphire_path_join(const char* part1, const char* part2);
    const char* sapphire_path_basename(const char* path);
    const char* sapphire_path_dirname(const char* path);
}

#endif // SAPPHIRE_STDLIB_FILE_H
