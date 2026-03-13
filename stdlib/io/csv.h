#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace sapphire {
namespace stdlib {
namespace IO {

// CSV Data Structure
struct CSVData {
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> headers;
    bool has_headers;
    
    CSVData() : has_headers(false) {}
    
    size_t row_count() const { return rows.size(); }
    size_t col_count() const { return rows.empty() ? 0 : rows[0].size(); }
    
    std::string get(size_t row, size_t col) const;
    void set(size_t row, size_t col, const std::string& value);
    void add_row(const std::vector<std::string>& row);
    void clear();
};

// CSV Parser
class CSVParser {
private:
    char delimiter;
    char quote_char;
    bool skip_empty_lines;
    
public:
    CSVParser(char delim = ',', char quote = '"', bool skip_empty = true);
    
    CSVData* parse_file(const std::string& filename, bool has_headers = true);
    CSVData* parse_string(const std::string& content, bool has_headers = true);
    
    bool write_file(const std::string& filename, const CSVData* data);
    std::string write_string(const CSVData* data);
    
private:
    std::vector<std::string> parse_line(const std::string& line);
    std::string escape_field(const std::string& field);
};

// Factory functions
CSVData* create_csv_data();
void destroy_csv_data(CSVData* data);
CSVParser* create_csv_parser(char delimiter = ',');
void destroy_csv_parser(CSVParser* parser);

// Convenience functions
CSVData* csv_read_file(const std::string& filename, bool has_headers = true);
bool csv_write_file(const std::string& filename, const CSVData* data);
CSVData* csv_parse_string(const std::string& content, bool has_headers = true);
std::string csv_to_string(const CSVData* data);

} // namespace IO
} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // CSV Data lifecycle
    void* sapphire_csv_create();
    void sapphire_csv_destroy(void* csv_data);
    
    // File operations
    void* sapphire_csv_read_file(const char* filename, bool has_headers);
    bool sapphire_csv_write_file(const char* filename, void* csv_data);
    
    // String operations
    void* sapphire_csv_parse_string(const char* content, bool has_headers);
    const char* sapphire_csv_to_string(void* csv_data);
    
    // Data access
    int64_t sapphire_csv_row_count(void* csv_data);
    int64_t sapphire_csv_col_count(void* csv_data);
    const char* sapphire_csv_get(void* csv_data, int64_t row, int64_t col);
    void sapphire_csv_set(void* csv_data, int64_t row, int64_t col, const char* value);
    
    // Headers
    const char* sapphire_csv_get_header(void* csv_data, int64_t col);
    void sapphire_csv_set_header(void* csv_data, int64_t col, const char* value);
    bool sapphire_csv_has_headers(void* csv_data);
    
    // Row operations
    void sapphire_csv_add_row(void* csv_data);
    void sapphire_csv_clear(void* csv_data);
}