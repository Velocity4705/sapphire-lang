#include "text.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <cstdarg>
#include <map>
#include <cstring>

namespace sapphire {
namespace stdlib {
namespace Text {

//=============================================================================
// Regex Implementation
//=============================================================================

Regex::Regex(const std::string& pattern) : pattern_str(pattern) {
    try {
        this->pattern = std::regex(pattern_str, std::regex_constants::ECMAScript);
    } catch (const std::regex_error& e) {
        throw std::runtime_error("Invalid regex pattern: " + pattern_str + " - " + e.what());
    }
}

bool Regex::matches(const std::string& text) const {
    return std::regex_match(text, pattern);
}

bool Regex::search(const std::string& text) const {
    return std::regex_search(text, pattern);
}

std::vector<std::string> Regex::find_all(const std::string& text) const {
    std::vector<std::string> results;
    std::sregex_iterator iter(text.begin(), text.end(), pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        results.push_back(iter->str());
    }
    
    return results;
}

std::string Regex::replace(const std::string& text, const std::string& replacement) const {
    return std::regex_replace(text, pattern, replacement, std::regex_constants::format_first_only);
}

std::string Regex::replace_all(const std::string& text, const std::string& replacement) const {
    return std::regex_replace(text, pattern, replacement);
}

std::vector<Regex::Match> Regex::find_matches(const std::string& text) const {
    std::vector<Match> matches;
    std::sregex_iterator iter(text.begin(), text.end(), pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        Match m;
        m.full_match = match.str();
        m.start_pos = match.position();
        m.end_pos = match.position() + match.length();
        
        // Extract groups
        for (size_t i = 1; i < match.size(); ++i) {
            m.groups.push_back(match[i].str());
        }
        
        matches.push_back(m);
    }
    
    return matches;
}

//=============================================================================
// TextProcessor Implementation
//=============================================================================

size_t TextProcessor::word_count(const std::string& text) {
    if (text.empty()) return 0;
    
    size_t count = 0;
    bool in_word = false;
    
    for (char c : text) {
        if (std::isspace(c)) {
            in_word = false;
        } else if (!in_word) {
            in_word = true;
            count++;
        }
    }
    
    return count;
}

size_t TextProcessor::line_count(const std::string& text) {
    if (text.empty()) return 0;
    
    size_t count = 1;
    for (char c : text) {
        if (c == '\n') count++;
    }
    
    return count;
}

size_t TextProcessor::char_count(const std::string& text) {
    return text.length();
}

std::vector<std::string> TextProcessor::extract_words(const std::string& text) {
    std::vector<std::string> words;
    std::string current_word;
    
    for (char c : text) {
        if (std::isspace(c) || std::ispunct(c)) {
            if (!current_word.empty()) {
                words.push_back(current_word);
                current_word.clear();
            }
        } else {
            current_word += c;
        }
    }
    
    if (!current_word.empty()) {
        words.push_back(current_word);
    }
    
    return words;
}

std::vector<std::string> TextProcessor::extract_lines(const std::string& text) {
    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string line;
    
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

std::string TextProcessor::normalize_whitespace(const std::string& text) {
    std::string result;
    bool prev_space = false;
    
    for (char c : text) {
        if (std::isspace(c)) {
            if (!prev_space) {
                result += ' ';
                prev_space = true;
            }
        } else {
            result += c;
            prev_space = false;
        }
    }
    
    // Trim leading/trailing spaces
    if (!result.empty() && result.front() == ' ') {
        result.erase(0, 1);
    }
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string TextProcessor::remove_punctuation(const std::string& text) {
    std::string result;
    for (char c : text) {
        if (!std::ispunct(c)) {
            result += c;
        }
    }
    return result;
}

std::string TextProcessor::capitalize_words(const std::string& text) {
    std::string result = text;
    bool capitalize_next = true;
    
    for (char& c : result) {
        if (std::isspace(c)) {
            capitalize_next = true;
        } else if (capitalize_next) {
            c = std::toupper(c);
            capitalize_next = false;
        } else {
            c = std::tolower(c);
        }
    }
    
    return result;
}

std::string TextProcessor::reverse_text(const std::string& text) {
    std::string result = text;
    std::reverse(result.begin(), result.end());
    return result;
}

std::string TextProcessor::wrap_text(const std::string& text, size_t width) {
    if (width == 0) return text;
    
    std::vector<std::string> words = extract_words(text);
    std::string result;
    std::string current_line;
    
    for (const auto& word : words) {
        if (current_line.empty()) {
            current_line = word;
        } else if (current_line.length() + 1 + word.length() <= width) {
            current_line += " " + word;
        } else {
            result += current_line + "\n";
            current_line = word;
        }
    }
    
    if (!current_line.empty()) {
        result += current_line;
    }
    
    return result;
}

std::string TextProcessor::indent_text(const std::string& text, size_t spaces) {
    std::string indent(spaces, ' ');
    std::string result;
    std::stringstream ss(text);
    std::string line;
    
    while (std::getline(ss, line)) {
        result += indent + line + "\n";
    }
    
    // Remove trailing newline if original didn't have one
    if (!text.empty() && text.back() != '\n' && !result.empty()) {
        result.pop_back();
    }
    
    return result;
}

std::string TextProcessor::center_text(const std::string& text, size_t width) {
    if (text.length() >= width) return text;
    
    size_t padding = (width - text.length()) / 2;
    return std::string(padding, ' ') + text + std::string(width - text.length() - padding, ' ');
}

std::string TextProcessor::pad_left(const std::string& text, size_t width, char pad_char) {
    if (text.length() >= width) return text;
    return std::string(width - text.length(), pad_char) + text;
}

std::string TextProcessor::pad_right(const std::string& text, size_t width, char pad_char) {
    if (text.length() >= width) return text;
    return text + std::string(width - text.length(), pad_char);
}

double TextProcessor::similarity(const std::string& text1, const std::string& text2) {
    if (text1.empty() && text2.empty()) return 1.0;
    if (text1.empty() || text2.empty()) return 0.0;
    
    size_t distance = levenshtein_distance(text1, text2);
    size_t max_len = std::max(text1.length(), text2.length());
    
    return 1.0 - (static_cast<double>(distance) / max_len);
}

size_t TextProcessor::levenshtein_distance(const std::string& text1, const std::string& text2) {
    const size_t len1 = text1.length();
    const size_t len2 = text2.length();
    
    std::vector<std::vector<size_t>> matrix(len1 + 1, std::vector<size_t>(len2 + 1));
    
    // Initialize first row and column
    for (size_t i = 0; i <= len1; ++i) matrix[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) matrix[0][j] = j;
    
    // Fill the matrix
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            size_t cost = (text1[i-1] == text2[j-1]) ? 0 : 1;
            matrix[i][j] = std::min({
                matrix[i-1][j] + 1,      // deletion
                matrix[i][j-1] + 1,      // insertion
                matrix[i-1][j-1] + cost  // substitution
            });
        }
    }
    
    return matrix[len1][len2];
}

bool TextProcessor::fuzzy_match(const std::string& pattern, const std::string& text, double threshold) {
    return similarity(pattern, text) >= threshold;
}

//=============================================================================
// StringBuilder Implementation
//=============================================================================

StringBuilder::StringBuilder() : total_length(0) {}

StringBuilder& StringBuilder::append(const std::string& text) {
    parts.push_back(text);
    total_length += text.length();
    return *this;
}

StringBuilder& StringBuilder::append(char c) {
    parts.push_back(std::string(1, c));
    total_length += 1;
    return *this;
}

StringBuilder& StringBuilder::append_line(const std::string& text) {
    parts.push_back(text + "\n");
    total_length += text.length() + 1;
    return *this;
}

StringBuilder& StringBuilder::append_format(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // Get required size
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);
    
    if (size > 0) {
        std::string buffer(size, '\0');
        vsnprintf(&buffer[0], size + 1, format, args);
        append(buffer);
    }
    
    va_end(args);
    return *this;
}

std::string StringBuilder::to_string() const {
    std::string result;
    result.reserve(total_length);
    
    for (const auto& part : parts) {
        result += part;
    }
    
    return result;
}

void StringBuilder::clear() {
    parts.clear();
    total_length = 0;
}

//=============================================================================
// Template Implementation
//=============================================================================

Template::Template(const std::string& template_str) : template_text(template_str) {}

void Template::set_variable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

void Template::set_variable(const std::string& name, int value) {
    variables[name] = std::to_string(value);
}

void Template::set_variable(const std::string& name, double value) {
    variables[name] = std::to_string(value);
}

void Template::set_variable(const std::string& name, bool value) {
    variables[name] = value ? "true" : "false";
}

std::string Template::render() const {
    return render(variables);
}

std::string Template::render(const std::map<std::string, std::string>& vars) const {
    std::string result = template_text;
    
    // Simple variable substitution: {{variable_name}}
    for (const auto& var : vars) {
        std::string placeholder = "{{" + var.first + "}}";
        size_t pos = 0;
        
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    return result;
}

//=============================================================================
// Factory Functions
//=============================================================================

std::unique_ptr<Regex> create_regex(const std::string& pattern) {
    return std::make_unique<Regex>(pattern);
}

std::unique_ptr<StringBuilder> create_string_builder() {
    return std::make_unique<StringBuilder>();
}

std::unique_ptr<Template> create_template(const std::string& template_str) {
    return std::make_unique<Template>(template_str);
}

} // namespace Text
} // namespace stdlib
} // namespace sapphire

//=============================================================================
// C API Implementation
//=============================================================================

using namespace sapphire::stdlib::Text;

extern "C" {
    static thread_local char g_text_string_buffer[8192];
    
    void* sapphire_regex_create(const char* pattern) {
        try {
            return new Regex(pattern);
        } catch (...) {
            return nullptr;
        }
    }
    
    void sapphire_regex_destroy(void* regex) {
        delete static_cast<Regex*>(regex);
    }
    
    bool sapphire_regex_matches(void* regex, const char* text) {
        if (!regex || !text) return false;
        try {
            return static_cast<Regex*>(regex)->matches(text);
        } catch (...) {
            return false;
        }
    }
    
    const char* sapphire_regex_replace(void* regex, const char* text, const char* replacement) {
        if (!regex || !text || !replacement) return nullptr;
        try {
            std::string result = static_cast<Regex*>(regex)->replace(text, replacement);
            strncpy(g_text_string_buffer, result.c_str(), sizeof(g_text_string_buffer) - 1);
            g_text_string_buffer[sizeof(g_text_string_buffer) - 1] = '\0';
            return g_text_string_buffer;
        } catch (...) {
            return nullptr;
        }
    }
    
    size_t sapphire_text_word_count(const char* text) {
        if (!text) return 0;
        return TextProcessor::word_count(text);
    }
    
    size_t sapphire_text_line_count(const char* text) {
        if (!text) return 0;
        return TextProcessor::line_count(text);
    }
    
    const char* sapphire_text_normalize_whitespace(const char* text) {
        if (!text) return nullptr;
        std::string result = TextProcessor::normalize_whitespace(text);
        strncpy(g_text_string_buffer, result.c_str(), sizeof(g_text_string_buffer) - 1);
        g_text_string_buffer[sizeof(g_text_string_buffer) - 1] = '\0';
        return g_text_string_buffer;
    }
    
    const char* sapphire_text_capitalize_words(const char* text) {
        if (!text) return nullptr;
        std::string result = TextProcessor::capitalize_words(text);
        strncpy(g_text_string_buffer, result.c_str(), sizeof(g_text_string_buffer) - 1);
        g_text_string_buffer[sizeof(g_text_string_buffer) - 1] = '\0';
        return g_text_string_buffer;
    }
    
    void* sapphire_string_builder_create() {
        return new StringBuilder();
    }
    
    void sapphire_string_builder_destroy(void* builder) {
        delete static_cast<StringBuilder*>(builder);
    }
    
    void sapphire_string_builder_append(void* builder, const char* text) {
        if (builder && text) {
            static_cast<StringBuilder*>(builder)->append(text);
        }
    }
    
    const char* sapphire_string_builder_to_string(void* builder) {
        if (!builder) return nullptr;
        std::string result = static_cast<StringBuilder*>(builder)->to_string();
        strncpy(g_text_string_buffer, result.c_str(), sizeof(g_text_string_buffer) - 1);
        g_text_string_buffer[sizeof(g_text_string_buffer) - 1] = '\0';
        return g_text_string_buffer;
    }
    
    void* sapphire_template_create(const char* template_str) {
        if (!template_str) return nullptr;
        return new Template(template_str);
    }
    
    void sapphire_template_destroy(void* template_obj) {
        delete static_cast<Template*>(template_obj);
    }
    
    void sapphire_template_set_variable(void* template_obj, const char* name, const char* value) {
        if (template_obj && name && value) {
            static_cast<Template*>(template_obj)->set_variable(std::string(name), std::string(value));
        }
    }
    
    const char* sapphire_template_render(void* template_obj) {
        if (!template_obj) return nullptr;
        std::string result = static_cast<Template*>(template_obj)->render();
        strncpy(g_text_string_buffer, result.c_str(), sizeof(g_text_string_buffer) - 1);
        g_text_string_buffer[sizeof(g_text_string_buffer) - 1] = '\0';
        return g_text_string_buffer;
    }
}