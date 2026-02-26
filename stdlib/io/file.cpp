#include "file.h"
#include <cstring>

namespace sapphire {
namespace stdlib {

File::File() : file_(nullptr), is_open_(false) {}

File::File(File&& other) noexcept 
    : file_(other.file_), path_(std::move(other.path_)), 
      mode_(std::move(other.mode_)), is_open_(other.is_open_) {
    other.file_ = nullptr;
    other.is_open_ = false;
}

File& File::operator=(File&& other) noexcept {
    if (this != &other) {
        close();
        file_ = other.file_;
        path_ = std::move(other.path_);
        mode_ = std::move(other.mode_);
        is_open_ = other.is_open_;
        other.file_ = nullptr;
        other.is_open_ = false;
    }
    return *this;
}

File::~File() {
    close();
}

File File::open(const String& path, const String& mode) {
    File file;
    file.open_file(path, mode);
    return file;
}

File File::open(const char* path, const char* mode) {
    return open(String(path), String(mode));
}

bool File::open_file(const String& path, const String& mode) {
    close();
    
    file_ = std::fopen(path.c_str(), mode.c_str());
    if (file_) {
        path_ = path;
        mode_ = mode;
        is_open_ = true;
        return true;
    }
    return false;
}

void File::close() {
    if (file_) {
        std::fclose(file_);
        file_ = nullptr;
        is_open_ = false;
    }
}

String File::read() {
    if (!is_open_) return String();
    
    std::fseek(file_, 0, SEEK_END);
    long size = std::ftell(file_);
    std::fseek(file_, 0, SEEK_SET);
    
    char* buffer = new char[size + 1];
    size_t read_size = std::fread(buffer, 1, size, file_);
    buffer[read_size] = '\0';
    
    String result(buffer);
    delete[] buffer;
    return result;
}

String File::read(size_t size) {
    if (!is_open_) return String();
    
    char* buffer = new char[size + 1];
    size_t read_size = std::fread(buffer, 1, size, file_);
    buffer[read_size] = '\0';
    
    String result(buffer);
    delete[] buffer;
    return result;
}

String File::read_line() {
    if (!is_open_) return String();
    
    char buffer[1024];
    if (std::fgets(buffer, sizeof(buffer), file_)) {
        // Remove newline
        size_t len = std::strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        return String(buffer);
    }
    return String();
}

Vec<String> File::read_lines() {
    Vec<String> lines;
    while (!std::feof(file_)) {
        String line = read_line();
        if (!line.empty() || !std::feof(file_)) {
            lines.push(line);
        }
    }
    return lines;
}

void File::write(const String& data) {
    if (is_open_) {
        std::fwrite(data.c_str(), 1, data.length(), file_);
    }
}

void File::write_line(const String& line) {
    write(line);
    write(String("\n"));
}

void File::flush() {
    if (is_open_) {
        std::fflush(file_);
    }
}

bool File::exists(const String& path) {
    FILE* file = std::fopen(path.c_str(), "r");
    if (file) {
        std::fclose(file);
        return true;
    }
    return false;
}

bool File::remove_file(const String& path) {
    return std::remove(path.c_str()) == 0;
}

String File::read_all(const String& path) {
    File file = File::open(path, "r");
    if (!file.is_open()) return String();
    String content = file.read();
    file.close();
    return content;
}

void File::write_all(const String& path, const String& data) {
    File file = File::open(path, "w");
    if (file.is_open()) {
        file.write(data);
        file.close();
    }
}

} // namespace stdlib
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

using namespace sapphire::stdlib;

extern "C" {

void* sapphire_file_open(const char* path, const char* mode) {
    File* file = new File();
    if (file->open_file(String(path), String(mode))) {
        return file;
    }
    delete file;
    return nullptr;
}

void sapphire_file_close(void* file) {
    if (file) {
        static_cast<File*>(file)->close();
        delete static_cast<File*>(file);
    }
}

const char* sapphire_file_read(void* file) {
    if (!file) return "";
    static String result = static_cast<File*>(file)->read();
    return result.c_str();
}

void sapphire_file_write(void* file, const char* data) {
    if (file) {
        static_cast<File*>(file)->write(String(data));
    }
}

} // extern "C"
