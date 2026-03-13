#include "stream.h"
#include <stdexcept>
#include <cstring>

namespace sapphire {
namespace stdlib {
namespace IO {

//=============================================================================
// FileStream Implementation
//=============================================================================

FileStream::FileStream(const std::string& path, const std::string& mode) 
    : filename(path), mode(mode) {
    
    std::ios_base::openmode flags = std::ios_base::binary;
    
    if (mode.find('r') != std::string::npos) {
        flags |= std::ios_base::in;
    }
    if (mode.find('w') != std::string::npos) {
        flags |= std::ios_base::out | std::ios_base::trunc;
    }
    if (mode.find('a') != std::string::npos) {
        flags |= std::ios_base::out | std::ios_base::app;
    }
    if (mode.find('+') != std::string::npos) {
        flags |= std::ios_base::in | std::ios_base::out;
    }
    
    file.open(path, flags);
    if (!file.is_open()) {
        last_error = "Failed to open file: " + path;
    }
}

FileStream::~FileStream() {
    if (file.is_open()) {
        file.close();
    }
}

bool FileStream::is_open() const {
    return file.is_open();
}

void FileStream::close() {
    if (file.is_open()) {
        file.close();
    }
}

bool FileStream::eof() const {
    return file.eof();
}

size_t FileStream::position() const {
    return static_cast<size_t>(const_cast<std::fstream&>(file).tellg());
}

void FileStream::seek(size_t pos) {
    file.seekg(pos);
    file.seekp(pos);
}

bool FileStream::has_error() const {
    return !last_error.empty() || file.fail();
}

std::string FileStream::error_message() const {
    if (!last_error.empty()) {
        return last_error;
    }
    if (file.fail()) {
        return "Stream operation failed";
    }
    return "";
}

std::string FileStream::read(size_t bytes) {
    if (!is_open()) {
        last_error = "Stream not open for reading";
        return "";
    }
    
    std::string result(bytes, '\0');
    file.read(&result[0], bytes);
    size_t actual_read = file.gcount();
    result.resize(actual_read);
    
    return result;
}

std::string FileStream::read_line() {
    if (!is_open()) {
        last_error = "Stream not open for reading";
        return "";
    }
    
    std::string line;
    std::getline(file, line);
    return line;
}

std::vector<std::string> FileStream::read_lines(size_t max_lines) {
    std::vector<std::string> lines;
    std::string line;
    size_t count = 0;
    
    while (std::getline(file, line) && (max_lines == 0 || count < max_lines)) {
        lines.push_back(line);
        count++;
    }
    
    return lines;
}

char FileStream::read_char() {
    if (!is_open()) {
        last_error = "Stream not open for reading";
        return '\0';
    }
    
    return static_cast<char>(file.get());
}

void FileStream::write(const std::string& data) {
    if (!is_open()) {
        last_error = "Stream not open for writing";
        return;
    }
    
    file.write(data.c_str(), data.length());
}

void FileStream::write_line(const std::string& line) {
    write(line + "\n");
}

void FileStream::write_char(char c) {
    if (!is_open()) {
        last_error = "Stream not open for writing";
        return;
    }
    
    file.put(c);
}

void FileStream::flush() {
    if (is_open()) {
        file.flush();
    }
}

//=============================================================================
// BufferedReader Implementation
//=============================================================================

BufferedReader::BufferedReader(const std::string& path, size_t buffer_size) 
    : buffer_size(buffer_size), buffer_pos(0), buffer_end(0) {
    
    stream = std::make_unique<FileStream>(path, "rb");
    buffer.resize(buffer_size);
    
    if (!stream->is_open()) {
        throw std::runtime_error("Failed to open file for buffered reading: " + path);
    }
}

void BufferedReader::fill_buffer() {
    if (stream->eof()) return;
    
    std::string data = stream->read(buffer_size);
    buffer_end = data.length();
    buffer_pos = 0;
    
    if (buffer_end > 0) {
        std::memcpy(buffer.data(), data.c_str(), buffer_end);
    }
}

char BufferedReader::read_char() {
    if (buffer_pos >= buffer_end) {
        fill_buffer();
        if (buffer_end == 0) {
            return '\0';  // EOF
        }
    }
    
    return buffer[buffer_pos++];
}

std::string BufferedReader::read_line() {
    std::string line;
    char c;
    
    while ((c = read_char()) != '\0') {
        if (c == '\n') {
            break;
        }
        if (c != '\r') {  // Skip carriage returns
            line += c;
        }
    }
    
    return line;
}

std::string BufferedReader::read(size_t bytes) {
    std::string result;
    result.reserve(bytes);
    
    for (size_t i = 0; i < bytes; ++i) {
        char c = read_char();
        if (c == '\0') break;
        result += c;
    }
    
    return result;
}

bool BufferedReader::eof() const {
    return buffer_pos >= buffer_end && stream->eof();
}

void BufferedReader::close() {
    if (stream) {
        stream->close();
    }
}

//=============================================================================
// BufferedWriter Implementation
//=============================================================================

BufferedWriter::BufferedWriter(const std::string& path, size_t buffer_size) 
    : buffer_size(buffer_size), buffer_pos(0) {
    
    stream = std::make_unique<FileStream>(path, "wb");
    buffer.resize(buffer_size);
    
    if (!stream->is_open()) {
        throw std::runtime_error("Failed to open file for buffered writing: " + path);
    }
}

BufferedWriter::~BufferedWriter() {
    try {
        flush();
        close();
    } catch (...) {
        // Don't throw in destructor
    }
}

void BufferedWriter::flush_buffer() {
    if (buffer_pos > 0 && stream->is_open()) {
        std::string data(buffer.data(), buffer_pos);
        stream->write(data);
        stream->flush();
        buffer_pos = 0;
    }
}

void BufferedWriter::write_char(char c) {
    if (buffer_pos >= buffer_size) {
        flush_buffer();
    }
    
    buffer[buffer_pos++] = c;
}

void BufferedWriter::write(const std::string& data) {
    for (char c : data) {
        write_char(c);
    }
}

void BufferedWriter::write_line(const std::string& line) {
    write(line + "\n");
}

void BufferedWriter::flush() {
    flush_buffer();
}

void BufferedWriter::close() {
    flush();
    if (stream) {
        stream->close();
    }
}

//=============================================================================
// Factory Functions
//=============================================================================

std::unique_ptr<FileStream> create_file_stream(const std::string& path, const std::string& mode) {
    return std::make_unique<FileStream>(path, mode);
}

std::unique_ptr<BufferedReader> create_buffered_reader(const std::string& path) {
    return std::make_unique<BufferedReader>(path);
}

std::unique_ptr<BufferedWriter> create_buffered_writer(const std::string& path) {
    return std::make_unique<BufferedWriter>(path);
}

} // namespace IO
} // namespace stdlib
} // namespace sapphire

//=============================================================================
// C API Implementation
//=============================================================================

using namespace sapphire::stdlib::IO;

extern "C" {
    static thread_local char g_stream_string_buffer[8192];
    
    void* sapphire_file_stream_create(const char* path, const char* mode) {
        if (!path || !mode) return nullptr;
        try {
            return new FileStream(path, mode);
        } catch (...) {
            return nullptr;
        }
    }
    
    void sapphire_file_stream_destroy(void* stream) {
        delete static_cast<FileStream*>(stream);
    }
    
    const char* sapphire_file_stream_read(void* stream, size_t bytes) {
        if (!stream) return nullptr;
        try {
            std::string result = static_cast<FileStream*>(stream)->read(bytes);
            strncpy(g_stream_string_buffer, result.c_str(), sizeof(g_stream_string_buffer) - 1);
            g_stream_string_buffer[sizeof(g_stream_string_buffer) - 1] = '\0';
            return g_stream_string_buffer;
        } catch (...) {
            return nullptr;
        }
    }
    
    const char* sapphire_file_stream_read_line(void* stream) {
        if (!stream) return nullptr;
        try {
            std::string result = static_cast<FileStream*>(stream)->read_line();
            strncpy(g_stream_string_buffer, result.c_str(), sizeof(g_stream_string_buffer) - 1);
            g_stream_string_buffer[sizeof(g_stream_string_buffer) - 1] = '\0';
            return g_stream_string_buffer;
        } catch (...) {
            return nullptr;
        }
    }
    
    void sapphire_file_stream_write(void* stream, const char* data) {
        if (stream && data) {
            try {
                static_cast<FileStream*>(stream)->write(data);
            } catch (...) {
                // Silently fail
            }
        }
    }
    
    void sapphire_file_stream_close(void* stream) {
        if (stream) {
            static_cast<FileStream*>(stream)->close();
        }
    }
    
    void* sapphire_buffered_reader_create(const char* path) {
        if (!path) return nullptr;
        try {
            return new BufferedReader(path);
        } catch (...) {
            return nullptr;
        }
    }
    
    void sapphire_buffered_reader_destroy(void* reader) {
        delete static_cast<BufferedReader*>(reader);
    }
    
    const char* sapphire_buffered_reader_read_line(void* reader) {
        if (!reader) return nullptr;
        try {
            std::string result = static_cast<BufferedReader*>(reader)->read_line();
            strncpy(g_stream_string_buffer, result.c_str(), sizeof(g_stream_string_buffer) - 1);
            g_stream_string_buffer[sizeof(g_stream_string_buffer) - 1] = '\0';
            return g_stream_string_buffer;
        } catch (...) {
            return nullptr;
        }
    }
    
    char sapphire_buffered_reader_read_char(void* reader) {
        if (!reader) return '\0';
        try {
            return static_cast<BufferedReader*>(reader)->read_char();
        } catch (...) {
            return '\0';
        }
    }
    
    void* sapphire_buffered_writer_create(const char* path) {
        if (!path) return nullptr;
        try {
            return new BufferedWriter(path);
        } catch (...) {
            return nullptr;
        }
    }
    
    void sapphire_buffered_writer_destroy(void* writer) {
        delete static_cast<BufferedWriter*>(writer);
    }
    
    void sapphire_buffered_writer_write(void* writer, const char* data) {
        if (writer && data) {
            try {
                static_cast<BufferedWriter*>(writer)->write(data);
            } catch (...) {
                // Silently fail
            }
        }
    }
    
    void sapphire_buffered_writer_write_line(void* writer, const char* line) {
        if (writer && line) {
            try {
                static_cast<BufferedWriter*>(writer)->write_line(line);
            } catch (...) {
                // Silently fail
            }
        }
    }
    
    void sapphire_buffered_writer_flush(void* writer) {
        if (writer) {
            try {
                static_cast<BufferedWriter*>(writer)->flush();
            } catch (...) {
                // Silently fail
            }
        }
    }
}