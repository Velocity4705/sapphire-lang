#ifndef SAPPHIRE_STDLIB_REGEX_H
#define SAPPHIRE_STDLIB_REGEX_H

#include <string>
#include <vector>
#include <regex>

namespace sapphire {
namespace stdlib {

/**
 * Match - Represents a regex match
 */
struct Match {
    std::string text;           // Matched text
    size_t position;            // Position in original string
    size_t length;              // Length of match
    std::vector<std::string> groups;  // Capture groups
    
    Match() : position(0), length(0) {}
};

/**
 * Regex - Regular expression matching
 * 
 * Provides pattern matching and text manipulation using regular expressions.
 */
class Regex {
public:
    // Constructors
    Regex();
    explicit Regex(const std::string& pattern);
    Regex(const std::string& pattern, bool case_insensitive);
    ~Regex();
    
    // Compile pattern
    void compile(const std::string& pattern);
    void compile(const std::string& pattern, bool case_insensitive);
    
    // Matching
    bool matches(const std::string& text) const;
    bool search(const std::string& text) const;
    Match find(const std::string& text) const;
    std::vector<Match> find_all(const std::string& text) const;
    
    // Replacement
    std::string replace(const std::string& text, const std::string& replacement) const;
    std::string replace_all(const std::string& text, const std::string& replacement) const;
    
    // Splitting
    std::vector<std::string> split(const std::string& text) const;
    
    // Static convenience methods
    static bool matches(const std::string& pattern, const std::string& text);
    static bool search(const std::string& pattern, const std::string& text);
    static Match find(const std::string& pattern, const std::string& text);
    static std::vector<Match> find_all(const std::string& pattern, const std::string& text);
    static std::string replace(const std::string& pattern, const std::string& text, const std::string& replacement);
    static std::string replace_all(const std::string& pattern, const std::string& text, const std::string& replacement);
    static std::vector<std::string> split(const std::string& pattern, const std::string& text);
    
    // Pattern validation
    static bool is_valid(const std::string& pattern);
    
    // Escape special characters
    static std::string escape(const std::string& str);
    
private:
    std::regex regex_;
    std::string pattern_;
    bool compiled_;
    
    Match create_match(const std::smatch& sm) const;
};

} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // Regex operations
    int sapphire_regex_matches(const char* pattern, const char* text);
    int sapphire_regex_search(const char* pattern, const char* text);
    const char* sapphire_regex_replace(const char* pattern, const char* text, const char* replacement);
    const char* sapphire_regex_replace_all(const char* pattern, const char* text, const char* replacement);
    int sapphire_regex_is_valid(const char* pattern);
}

#endif // SAPPHIRE_STDLIB_REGEX_H
