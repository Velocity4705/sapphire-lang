#include "file.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define mkdir(path, mode) _mkdir(path)
#define rmdir _rmdir
#define PATH_MAX MAX_PATH
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace sapphire {
namespace stdlib {

//=============================================================================
// File Implementation
//=============================================================================

std::string File::read(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> File::read_lines(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::vector<uint8_t> File::read_bytes(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Failed to read file: " + path);
    }
    
    return buffer;
}

void File::write(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path);
    }
    
    file << content;
}

void File::write_lines(const std::string& path, const std::vector<std::string>& lines) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path);
    }
    
    for (const auto& line : lines) {
        file << line << '\n';
    }
}

void File::write_bytes(const std::string& path, const std::vector<uint8_t>& bytes) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path);
    }
    
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}

void File::append(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for appending: " + path);
    }
    
    file << content;
}

bool File::exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0) && S_ISREG(buffer.st_mode);
}

void File::delete_file(const std::string& path) {
    if (std::remove(path.c_str()) != 0) {
        throw std::runtime_error("Failed to delete file: " + path);
    }
}

void File::copy(const std::string& source, const std::string& destination) {
    std::ifstream src(source, std::ios::binary);
    if (!src.is_open()) {
        throw std::runtime_error("Failed to open source file: " + source);
    }
    
    std::ofstream dst(destination, std::ios::binary);
    if (!dst.is_open()) {
        throw std::runtime_error("Failed to open destination file: " + destination);
    }
    
    dst << src.rdbuf();
}

void File::move(const std::string& source, const std::string& destination) {
    if (std::rename(source.c_str(), destination.c_str()) != 0) {
        // If rename fails, try copy and delete
        copy(source, destination);
        delete_file(source);
    }
}

size_t File::size(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        throw std::runtime_error("Failed to get file size: " + path);
    }
    return buffer.st_size;
}

//=============================================================================
// Dir Implementation
//=============================================================================

std::vector<std::string> Dir::list(const std::string& path) {
    std::vector<std::string> entries;
    
#ifdef _WIN32
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((path + "\\*").c_str(), &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open directory: " + path);
    }
    
    do {
        std::string name = findData.cFileName;
        if (name != "." && name != "..") {
            entries.push_back(name);
        }
    } while (FindNextFile(hFind, &findData) != 0);
    
    FindClose(hFind);
#else
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        throw std::runtime_error("Failed to open directory: " + path);
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            entries.push_back(name);
        }
    }
    
    closedir(dir);
#endif
    
    return entries;
}

bool Dir::exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0) && S_ISDIR(buffer.st_mode);
}

void Dir::create(const std::string& path) {
#ifdef _WIN32
    if (_mkdir(path.c_str()) != 0) {
#else
    if (mkdir(path.c_str(), 0755) != 0) {
#endif
        throw std::runtime_error("Failed to create directory: " + path);
    }
}

void Dir::create_all(const std::string& path) {
    std::string current;
    for (size_t i = 0; i < path.length(); ++i) {
        if (path[i] == '/' || path[i] == '\\') {
            if (!current.empty() && !exists(current)) {
                create(current);
            }
        }
        current += path[i];
    }
    
    if (!current.empty() && !exists(current)) {
        create(current);
    }
}

void Dir::remove(const std::string& path) {
    if (rmdir(path.c_str()) != 0) {
        throw std::runtime_error("Failed to remove directory: " + path);
    }
}

void Dir::remove_all(const std::string& path) {
    // List all entries
    auto entries = list(path);
    
    for (const auto& entry : entries) {
        std::string full_path = Path::join(path, entry);
        
        if (Dir::exists(full_path)) {
            remove_all(full_path);
        } else {
            File::delete_file(full_path);
        }
    }
    
    remove(path);
}

//=============================================================================
// Path Implementation
//=============================================================================

char Path::separator() {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}

std::string Path::join(const std::string& part1, const std::string& part2) {
    if (part1.empty()) return part2;
    if (part2.empty()) return part1;
    
    char sep = separator();
    if (part1.back() == '/' || part1.back() == '\\') {
        return part1 + part2;
    }
    return part1 + sep + part2;
}

std::string Path::join(const std::string& part1, const std::string& part2, const std::string& part3) {
    return join(join(part1, part2), part3);
}

std::string Path::join(const std::vector<std::string>& parts) {
    if (parts.empty()) return "";
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result = join(result, parts[i]);
    }
    return result;
}

std::string Path::basename(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

std::string Path::dirname(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return ".";
    }
    if (pos == 0) {
        return "/";
    }
    return path.substr(0, pos);
}

std::string Path::extension(const std::string& path) {
    std::string base = basename(path);
    size_t pos = base.find_last_of('.');
    if (pos == std::string::npos || pos == 0) {
        return "";
    }
    return base.substr(pos);
}

std::string Path::absolute(const std::string& path) {
    char buffer[PATH_MAX];
    
#ifdef _WIN32
    if (_fullpath(buffer, path.c_str(), PATH_MAX) == nullptr) {
        throw std::runtime_error("Failed to get absolute path: " + path);
    }
#else
    if (realpath(path.c_str(), buffer) == nullptr) {
        // If realpath fails, try to construct absolute path manually
        if (path[0] == '/') {
            return path;
        }
        
        if (getcwd(buffer, PATH_MAX) == nullptr) {
            throw std::runtime_error("Failed to get current directory");
        }
        return join(std::string(buffer), path);
    }
#endif
    
    return std::string(buffer);
}

} // namespace stdlib
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

using namespace sapphire::stdlib;

extern "C" {

const char* sapphire_file_read(const char* path) {
    static std::string result;
    try {
        result = File::read(path);
        return result.c_str();
    } catch (...) {
        return nullptr;
    }
}

void sapphire_file_write(const char* path, const char* content) {
    try {
        File::write(path, content);
    } catch (...) {
        // Silently fail
    }
}

int sapphire_file_exists(const char* path) {
    return File::exists(path) ? 1 : 0;
}

void sapphire_file_delete(const char* path) {
    try {
        File::delete_file(path);
    } catch (...) {
        // Silently fail
    }
}

void sapphire_file_copy(const char* source, const char* destination) {
    try {
        File::copy(source, destination);
    } catch (...) {
        // Silently fail
    }
}

size_t sapphire_file_size(const char* path) {
    try {
        return File::size(path);
    } catch (...) {
        return 0;
    }
}

int sapphire_dir_exists(const char* path) {
    return Dir::exists(path) ? 1 : 0;
}

void sapphire_dir_create(const char* path) {
    try {
        Dir::create(path);
    } catch (...) {
        // Silently fail
    }
}

void sapphire_dir_remove(const char* path) {
    try {
        Dir::remove(path);
    } catch (...) {
        // Silently fail
    }
}

const char* sapphire_path_join(const char* part1, const char* part2) {
    static std::string result;
    result = Path::join(part1, part2);
    return result.c_str();
}

const char* sapphire_path_basename(const char* path) {
    static std::string result;
    result = Path::basename(path);
    return result.c_str();
}

const char* sapphire_path_dirname(const char* path) {
    static std::string result;
    result = Path::dirname(path);
    return result.c_str();
}

} // extern "C"
