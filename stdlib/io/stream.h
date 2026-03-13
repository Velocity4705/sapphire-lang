#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <functional>

namespace sapphire {
namespace stdlib {
namespace IO {

/**
 * Stream - Base class for streaming I/O operations
 */
class Stream {
public:
    virtual ~Stream() = default;
    
    // Basic operations
    virtual bool is_open() const = 0;
    virtual void close() = 0;
    virtual bool eof() const = 0;
    virtual size_t position() const = 0;
    virtual void seek(size_t pos) = 0;
    
    // Error handling
    virtual bool has_error() const = 0;
    virtual std::string error_message() const = 0;
};

/**
 * FileStream - Streaming file I/O
 */
class FileStream : public Stream {
private:
    std::fstream file;
    std::string filename;
    std::string mode;
    std::string last_error;
    
public:
    FileStream(const std::string& path, const std::string& mode);
    ~FileStream() override;
    
    // Stream interface
    bool is_open() const override;
    void close() override;
    bool eof() const override;
    size_t position() const override;
    void seek(size_t pos) override;
    bool has_error() const override;
    std::string error_message() const override;
    
    // Reading
    std::string read(size_t bytes);
    std::string read_line();
    std::vector<std::string> read_lines(size_t max_lines = 0);
    char read_char();
    
    // Writing
    void write(const std::string& data);
    void write_line(const std::string& line);
    void write_char(char c);
    void flush();
};

/**
 * BufferedReader - Efficient buffered reading
 */
class BufferedReader {
private:
    std::unique_ptr<FileStream> stream;
    std::vector<char> buffer;
    size_t buffer_size;
    size_t buffer_pos;
    size_t buffer_end;
    
    void fill_buffer();
    
public:
    BufferedReader(const std::string& path, size_t buffer_size = 8192);
    ~BufferedReader() = default;
    
    // Reading operations
    char read_char();
    std::string read_line();
    std::string read(size_t bytes);
    bool eof() const;
    void close();
};

/**
 * BufferedWriter - Efficient buffered writing
 */
class BufferedWriter {
private:
    std::unique_ptr<FileStream> stream;
    std::vector<char> buffer;
    size_t buffer_size;
    size_t buffer_pos;
    
    void flush_buffer();
    
public:
    BufferedWriter(const std::string& path, size_t buffer_size = 8192);
    ~BufferedWriter();
    
    // Writing operations
    void write_char(char c);
    void write(const std::string& data);
    void write_line(const std::string& line);
    void flush();
    void close();
};

// Factory functions
std::unique_ptr<FileStream> create_file_stream(const std::string& path, const std::string& mode);
std::unique_ptr<BufferedReader> create_buffered_reader(const std::string& path);
std::unique_ptr<BufferedWriter> create_buffered_writer(const std::string& path);

} // namespace IO
} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // FileStream operations
    void* sapphire_file_stream_create(const char* path, const char* mode);
    void sapphire_file_stream_destroy(void* stream);
    const char* sapphire_file_stream_read(void* stream, size_t bytes);
    const char* sapphire_file_stream_read_line(void* stream);
    void sapphire_file_stream_write(void* stream, const char* data);
    void sapphire_file_stream_close(void* stream);
    
    // BufferedReader operations
    void* sapphire_buffered_reader_create(const char* path);
    void sapphire_buffered_reader_destroy(void* reader);
    const char* sapphire_buffered_reader_read_line(void* reader);
    char sapphire_buffered_reader_read_char(void* reader);
    
    // BufferedWriter operations
    void* sapphire_buffered_writer_create(const char* path);
    void sapphire_buffered_writer_destroy(void* writer);
    void sapphire_buffered_writer_write(void* writer, const char* data);
    void sapphire_buffered_writer_write_line(void* writer, const char* line);
    void sapphire_buffered_writer_flush(void* writer);
}