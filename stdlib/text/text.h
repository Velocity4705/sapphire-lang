#pragma once

#include <string>
#include <vector>
#include <regex>
#include <memory>
#include <map>

namespace sapphire {
namespace stdlib {
namespace Text {

/**
 * Regular Expression Support
 * Advanced pattern matching and text processing
 */
class Regex {
private:
    std::regex pattern;
    std::string pattern_str;
    
public:
    Regex(const std::string& pattern);
    ~Regex() = default;
    
    // Pattern matching
    bool matches(const std::string& text) const;
    bool search(const std::string& text) const;
    std::vector<std::string> find_all(const std::string& text) const;
    std::string replace(const std::string& text, const std::string& replacement) const;
    std::string replace_all(const std::string& text, const std::string& replacement) const;
    
    // Match groups
    struct Match {
        std::string full_match;
        std::vector<std::string> groups;
        size_t start_pos;
        size_t end_pos;
    };
    
    std::vector<Match> find_matches(const std::string& text) const;
    
    // Pattern info
    const std::string& pattern_string() const { return pattern_str; }
};

/**
 * Text Processing Utilities
 * Advanced string manipulation and analysis
 */
class TextProcessor {
public:
    // Text analysis
    static size_t word_count(const std::string& text);
    static size_t line_count(const std::string& text);
    static size_t char_count(const std::string& text);
    static std::vector<std::string> extract_words(const std::string& text);
    static std::vector<std::string> extract_lines(const std::string& text);
    
    // Text transformation
    static std::string normalize_whitespace(const std::string& text);
    static std::string remove_punctuation(const std::string& text);
    static std::string capitalize_words(const std::string& text);
    static std::string reverse_text(const std::string& text);
    
    // Text formatting
    static std::string wrap_text(const std::string& text, size_t width);
    static std::string indent_text(const std::string& text, size_t spaces);
    static std::string center_text(const std::string& text, size_t width);
    static std::string pad_left(const std::string& text, size_t width, char pad_char = ' ');
    static std::string pad_right(const std::string& text, size_t width, char pad_char = ' ');
    
    // Text comparison
    static double similarity(const std::string& text1, const std::string& text2);
    static size_t levenshtein_distance(const std::string& text1, const std::string& text2);
    static bool fuzzy_match(const std::string& pattern, const std::string& text, double threshold = 0.8);
};

/**
 * String Builder
 * Efficient string construction for large texts
 */
class StringBuilder {
private:
    std::vector<std::string> parts;
    size_t total_length;
    
public:
    StringBuilder();
    ~StringBuilder() = default;
    
    // Building
    StringBuilder& append(const std::string& text);
    StringBuilder& append(char c);
    StringBuilder& append_line(const std::string& text);
    StringBuilder& append_format(const char* format, ...);
    
    // Properties
    size_t length() const { return total_length; }
    bool empty() const { return total_length == 0; }
    
    // Output
    std::string to_string() const;
    void clear();
};

/**
 * Text Template Engine
 * Simple template processing with variable substitution
 */
class Template {
private:
    std::string template_text;
    std::map<std::string, std::string> variables;
    
public:
    Template(const std::string& template_str);
    ~Template() = default;
    
    // Variable management
    void set_variable(const std::string& name, const std::string& value);
    void set_variable(const std::string& name, int value);
    void set_variable(const std::string& name, double value);
    void set_variable(const std::string& name, bool value);
    
    // Rendering
    std::string render() const;
    std::string render(const std::map<std::string, std::string>& vars) const;
};

// Factory functions
std::unique_ptr<Regex> create_regex(const std::string& pattern);
std::unique_ptr<StringBuilder> create_string_builder();
std::unique_ptr<Template> create_template(const std::string& template_str);

} // namespace Text
} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // Regex operations
    void* sapphire_regex_create(const char* pattern);
    void sapphire_regex_destroy(void* regex);
    bool sapphire_regex_matches(void* regex, const char* text);
    const char* sapphire_regex_replace(void* regex, const char* text, const char* replacement);
    
    // Text processing
    size_t sapphire_text_word_count(const char* text);
    size_t sapphire_text_line_count(const char* text);
    const char* sapphire_text_normalize_whitespace(const char* text);
    const char* sapphire_text_capitalize_words(const char* text);
    
    // String builder
    void* sapphire_string_builder_create();
    void sapphire_string_builder_destroy(void* builder);
    void sapphire_string_builder_append(void* builder, const char* text);
    const char* sapphire_string_builder_to_string(void* builder);
    
    // Template engine
    void* sapphire_template_create(const char* template_str);
    void sapphire_template_destroy(void* template_obj);
    void sapphire_template_set_variable(void* template_obj, const char* name, const char* value);
    const char* sapphire_template_render(void* template_obj);
}