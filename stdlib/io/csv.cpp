#include "csv.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace sapphire {
namespace stdlib {
namespace IO {

// CSVData Implementation
std::string CSVData::get(size_t row, size_t col) const {
    if (row >= rows.size() || col >= rows[row].size()) {
        return "";
    }
    return rows[row][col];
}

void CSVData::set(size_t row, size_t col, const std::string& value) {
    // Ensure we have enough rows
    while (row >= rows.size()) {
        rows.emplace_back();
    }
    
    // Ensure the row has enough columns
    while (col >= rows[row].size()) {
        rows[row].emplace_back("");
    }
    
    rows[row][col] = value;
}

void CSVData::add_row(const std::vector<std::string>& row) {
    rows.push_back(row);
}

void CSVData::clear() {
    rows.clear();
    headers.clear();
    has_headers = false;
}

// CSVParser Implementation
CSVParser::CSVParser(char delim, char quote, bool skip_empty) 
    : delimiter(delim), quote_char(quote), skip_empty_lines(skip_empty) {
}

CSVData* CSVParser::parse_file(const std::string& filename, bool has_headers) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return nullptr;
    }
    
    CSVData* data = new CSVData();
    data->has_headers = has_headers;
    
    std::string line;
    bool first_line = true;
    
    while (std::getline(file, line)) {
        if (skip_empty_lines && line.empty()) {
            continue;
        }
        
        std::vector<std::string> fields = parse_line(line);
        
        if (first_line && has_headers) {
            data->headers = fields;
            first_line = false;
        } else {
            data->add_row(fields);
        }
    }
    
    return data;
}

CSVData* CSVParser::parse_string(const std::string& content, bool has_headers) {
    std::istringstream stream(content);
    CSVData* data = new CSVData();
    data->has_headers = has_headers;
    
    std::string line;
    bool first_line = true;
    
    while (std::getline(stream, line)) {
        if (skip_empty_lines && line.empty()) {
            continue;
        }
        
        std::vector<std::string> fields = parse_line(line);
        
        if (first_line && has_headers) {
            data->headers = fields;
            first_line = false;
        } else {
            data->add_row(fields);
        }
    }
    
    return data;
}

bool CSVParser::write_file(const std::string& filename, const CSVData* data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write headers if present
    if (data->has_headers && !data->headers.empty()) {
        for (size_t i = 0; i < data->headers.size(); ++i) {
            if (i > 0) file << delimiter;
            file << escape_field(data->headers[i]);
        }
        file << "\n";
    }
    
    // Write data rows
    for (const auto& row : data->rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) file << delimiter;
            file << escape_field(row[i]);
        }
        file << "\n";
    }
    
    return true;
}

std::string CSVParser::write_string(const CSVData* data) {
    std::ostringstream stream;
    
    // Write headers if present
    if (data->has_headers && !data->headers.empty()) {
        for (size_t i = 0; i < data->headers.size(); ++i) {
            if (i > 0) stream << delimiter;
            stream << escape_field(data->headers[i]);
        }
        stream << "\n";
    }
    
    // Write data rows
    for (const auto& row : data->rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) stream << delimiter;
            stream << escape_field(row[i]);
        }
        stream << "\n";
    }
    
    return stream.str();
}

std::vector<std::string> CSVParser::parse_line(const std::string& line) {
    std::vector<std::string> fields;
    std::string current_field;
    bool in_quotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (c == quote_char) {
            if (in_quotes) {
                if (i + 1 < line.length() && line[i + 1] == quote_char) {
                    // Escaped quote
                    current_field += quote_char;
                    ++i; // Skip next quote
                } else {
                    // End of quoted field
                    in_quotes = false;
                }
            } else {
                // Start of quoted field
                in_quotes = true;
            }
        } else if (c == delimiter && !in_quotes) {
            // Field separator
            fields.push_back(current_field);
            current_field.clear();
        } else {
            // Regular character
            current_field += c;
        }
    }
    
    // Add the last field
    fields.push_back(current_field);
    
    return fields;
}

std::string CSVParser::escape_field(const std::string& field) {
    bool needs_quotes = field.find(delimiter) != std::string::npos ||
                       field.find(quote_char) != std::string::npos ||
                       field.find('\n') != std::string::npos ||
                       field.find('\r') != std::string::npos;
    
    if (!needs_quotes) {
        return field;
    }
    
    std::string escaped = std::string(1, quote_char);
    for (char c : field) {
        if (c == quote_char) {
            escaped += quote_char; // Double the quote
        }
        escaped += c;
    }
    escaped += quote_char;
    
    return escaped;
}

// Factory functions
CSVData* create_csv_data() {
    return new CSVData();
}

void destroy_csv_data(CSVData* data) {
    delete data;
}

CSVParser* create_csv_parser(char delimiter) {
    return new CSVParser(delimiter);
}

void destroy_csv_parser(CSVParser* parser) {
    delete parser;
}

// Convenience functions
CSVData* csv_read_file(const std::string& filename, bool has_headers) {
    CSVParser parser;
    return parser.parse_file(filename, has_headers);
}

bool csv_write_file(const std::string& filename, const CSVData* data) {
    CSVParser parser;
    return parser.write_file(filename, data);
}

CSVData* csv_parse_string(const std::string& content, bool has_headers) {
    CSVParser parser;
    return parser.parse_string(content, has_headers);
}

std::string csv_to_string(const CSVData* data) {
    CSVParser parser;
    return parser.write_string(data);
}

} // namespace IO
} // namespace stdlib
} // namespace sapphire

// C API Implementation
extern "C" {
    static thread_local char g_csv_string_buffer[8192];
    
    void* sapphire_csv_create() {
        return sapphire::stdlib::IO::create_csv_data();
    }
    
    void sapphire_csv_destroy(void* csv_data) {
        if (csv_data) {
            sapphire::stdlib::IO::destroy_csv_data(
                (sapphire::stdlib::IO::CSVData*)csv_data);
        }
    }
    
    void* sapphire_csv_read_file(const char* filename, bool has_headers) {
        if (!filename) return nullptr;
        return sapphire::stdlib::IO::csv_read_file(filename, has_headers);
    }
    
    bool sapphire_csv_write_file(const char* filename, void* csv_data) {
        if (!filename || !csv_data) return false;
        return sapphire::stdlib::IO::csv_write_file(
            filename, (sapphire::stdlib::IO::CSVData*)csv_data);
    }
    
    void* sapphire_csv_parse_string(const char* content, bool has_headers) {
        if (!content) return nullptr;
        return sapphire::stdlib::IO::csv_parse_string(content, has_headers);
    }
    
    const char* sapphire_csv_to_string(void* csv_data) {
        if (!csv_data) return nullptr;
        
        std::string result = sapphire::stdlib::IO::csv_to_string(
            (sapphire::stdlib::IO::CSVData*)csv_data);
        
        strncpy(g_csv_string_buffer, result.c_str(), sizeof(g_csv_string_buffer) - 1);
        g_csv_string_buffer[sizeof(g_csv_string_buffer) - 1] = '\0';
        
        return g_csv_string_buffer;
    }
    
    int64_t sapphire_csv_row_count(void* csv_data) {
        if (!csv_data) return 0;
        return (int64_t)((sapphire::stdlib::IO::CSVData*)csv_data)->row_count();
    }
    
    int64_t sapphire_csv_col_count(void* csv_data) {
        if (!csv_data) return 0;
        return (int64_t)((sapphire::stdlib::IO::CSVData*)csv_data)->col_count();
    }
    
    const char* sapphire_csv_get(void* csv_data, int64_t row, int64_t col) {
        if (!csv_data || row < 0 || col < 0) return nullptr;
        
        std::string value = ((sapphire::stdlib::IO::CSVData*)csv_data)->get(
            (size_t)row, (size_t)col);
        
        strncpy(g_csv_string_buffer, value.c_str(), sizeof(g_csv_string_buffer) - 1);
        g_csv_string_buffer[sizeof(g_csv_string_buffer) - 1] = '\0';
        
        return g_csv_string_buffer;
    }
    
    void sapphire_csv_set(void* csv_data, int64_t row, int64_t col, const char* value) {
        if (!csv_data || row < 0 || col < 0 || !value) return;
        
        ((sapphire::stdlib::IO::CSVData*)csv_data)->set(
            (size_t)row, (size_t)col, value);
    }
    
    const char* sapphire_csv_get_header(void* csv_data, int64_t col) {
        if (!csv_data || col < 0) return nullptr;
        
        auto* data = (sapphire::stdlib::IO::CSVData*)csv_data;
        if (!data->has_headers || (size_t)col >= data->headers.size()) {
            return nullptr;
        }
        
        strncpy(g_csv_string_buffer, data->headers[col].c_str(), 
                sizeof(g_csv_string_buffer) - 1);
        g_csv_string_buffer[sizeof(g_csv_string_buffer) - 1] = '\0';
        
        return g_csv_string_buffer;
    }
    
    void sapphire_csv_set_header(void* csv_data, int64_t col, const char* value) {
        if (!csv_data || col < 0 || !value) return;
        
        auto* data = (sapphire::stdlib::IO::CSVData*)csv_data;
        data->has_headers = true;
        
        // Ensure headers vector is large enough
        while ((size_t)col >= data->headers.size()) {
            data->headers.emplace_back("");
        }
        
        data->headers[col] = value;
    }
    
    bool sapphire_csv_has_headers(void* csv_data) {
        if (!csv_data) return false;
        return ((sapphire::stdlib::IO::CSVData*)csv_data)->has_headers;
    }
    
    void sapphire_csv_add_row(void* csv_data) {
        if (!csv_data) return;
        ((sapphire::stdlib::IO::CSVData*)csv_data)->add_row(std::vector<std::string>());
    }
    
    void sapphire_csv_clear(void* csv_data) {
        if (!csv_data) return;
        ((sapphire::stdlib::IO::CSVData*)csv_data)->clear();
    }
}