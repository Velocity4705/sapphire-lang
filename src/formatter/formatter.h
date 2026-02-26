#ifndef SAPPHIRE_FORMATTER_H
#define SAPPHIRE_FORMATTER_H

#include <string>
#include <vector>

namespace sapphire {

struct FormatOptions {
    int indent_size = 4;
    int max_line_length = 100;
    bool space_around_operators = true;
    bool space_after_comma = true;
    bool newline_before_brace = false;
};

class Formatter {
private:
    FormatOptions options_;
    int current_indent_;
    std::string result_;
    
    void add_indent();
    void add_line(const std::string& line);
    void increase_indent();
    void decrease_indent();
    
public:
    explicit Formatter(const FormatOptions& options = FormatOptions());
    
    std::string format(const std::string& code);
    std::string format_file(const std::string& filename);
    bool format_file_inplace(const std::string& filename);
};

} // namespace sapphire

#endif // SAPPHIRE_FORMATTER_H
