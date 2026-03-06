#include "regex.h"
#include <stdexcept>

namespace sapphire {
namespace stdlib {

//=============================================================================
// Regex Implementation
//=============================================================================

Regex::Regex() : compiled_(false) {}

Regex::Regex(const std::string& pattern) : pattern_(pattern), compiled_(false) {
    compile(pattern);
}

Regex::Regex(const std::string& pattern, bool case_insensitive) 
    : pattern_(pattern), compiled_(false) {
    compile(pattern, case_insensitive);
}

Regex::~Regex() {}

void Regex::compile(const std::string& pattern) {
    try {
        regex_ = std::regex(pattern);
        pattern_ = pattern;
        compiled_ = true;
    } catch (const std::regex_error& e) {
        throw std::runtime_error("Invalid regex pattern: " + pattern);
    }
}

void Regex::compile(const std::string& pattern, bool case_insensitive) {
    try {
        auto flags = std::regex::ECMAScript;
        if (case_insensitive) {
            flags |= std::regex::icase;
        }
        regex_ = std::regex(pattern, flags);
        pattern_ = pattern;
        compiled_ = true;
    } catch (const std::regex_error& e) {
        throw std::runtime_error("Invalid regex pattern: " + pattern);
    }
}

bool Regex::matches(const std::string& text) const {
    if (!compiled_) return false;
    return std::regex_match(text, regex_);
}

bool Regex::search(const std::string& text) const {
    if (!compiled_) return false;
    return std::regex_search(text, regex_);
}

Match Regex::find(const std::string& text) const {
    if (!compiled_) return Match();
    
    std::smatch sm;
    if (std::regex_search(text, sm, regex_)) {
        return create_match(sm);
    }
    
    return Match();
}

std::vector<Match> Regex::find_all(const std::string& text) const {
    std::vector<Match> matches;
    if (!compiled_) return matches;
    
    std::string::const_iterator search_start(text.cbegin());
    std::smatch sm;
    
    while (std::regex_search(search_start, text.cend(), sm, regex_)) {
        matches.push_back(create_match(sm));
        search_start = sm.suffix().first;
    }
    
    return matches;
}

std::string Regex::replace(const std::string& text, const std::string& replacement) const {
    if (!compiled_) return text;
    
    std::smatch sm;
    if (std::regex_search(text, sm, regex_)) {
        return sm.prefix().str() + replacement + sm.suffix().str();
    }
    
    return text;
}

std::string Regex::replace_all(const std::string& text, const std::string& replacement) const {
    if (!compiled_) return text;
    return std::regex_replace(text, regex_, replacement);
}

std::vector<std::string> Regex::split(const std::string& text) const {
    std::vector<std::string> result;
    if (!compiled_) {
        result.push_back(text);
        return result;
    }
    
    std::sregex_token_iterator iter(text.begin(), text.end(), regex_, -1);
    std::sregex_token_iterator end;
    
    for (; iter != end; ++iter) {
        result.push_back(*iter);
    }
    
    return result;
}

Match Regex::create_match(const std::smatch& sm) const {
    Match match;
    match.text = sm.str();
    match.position = sm.position();
    match.length = sm.length();
    
    // Add capture groups
    for (size_t i = 1; i < sm.size(); ++i) {
        match.groups.push_back(sm[i].str());
    }
    
    return match;
}

//=============================================================================
// Static Methods
//=============================================================================

bool Regex::matches(const std::string& pattern, const std::string& text) {
    try {
        return std::regex_match(text, std::regex(pattern));
    } catch (...) {
        return false;
    }
}

bool Regex::search(const std::string& pattern, const std::string& text) {
    try {
        return std::regex_search(text, std::regex(pattern));
    } catch (...) {
        return false;
    }
}

Match Regex::find(const std::string& pattern, const std::string& text) {
    Regex regex(pattern);
    return regex.find(text);
}

std::vector<Match> Regex::find_all(const std::string& pattern, const std::string& text) {
    Regex regex(pattern);
    return regex.find_all(text);
}

std::string Regex::replace(const std::string& pattern, const std::string& text, 
                           const std::string& replacement) {
    Regex regex(pattern);
    return regex.replace(text, replacement);
}

std::string Regex::replace_all(const std::string& pattern, const std::string& text, 
                               const std::string& replacement) {
    Regex regex(pattern);
    return regex.replace_all(text, replacement);
}

std::vector<std::string> Regex::split(const std::string& pattern, const std::string& text) {
    Regex regex(pattern);
    return regex.split(text);
}

bool Regex::is_valid(const std::string& pattern) {
    try {
        std::regex test(pattern);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Regex::escape(const std::string& str) {
    static const std::string special_chars = "\\^$.|?*+()[]{}";
    std::string result;
    
    for (char c : str) {
        if (special_chars.find(c) != std::string::npos) {
            result += '\\';
        }
        result += c;
    }
    
    return result;
}

} // namespace stdlib
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

using namespace sapphire::stdlib;

extern "C" {

int sapphire_regex_matches(const char* pattern, const char* text) {
    return Regex::matches(pattern, text) ? 1 : 0;
}

int sapphire_regex_search(const char* pattern, const char* text) {
    return Regex::search(pattern, text) ? 1 : 0;
}

const char* sapphire_regex_replace(const char* pattern, const char* text, const char* replacement) {
    static std::string result;
    result = Regex::replace(pattern, text, replacement);
    return result.c_str();
}

const char* sapphire_regex_replace_all(const char* pattern, const char* text, const char* replacement) {
    static std::string result;
    result = Regex::replace_all(pattern, text, replacement);
    return result.c_str();
}

int sapphire_regex_is_valid(const char* pattern) {
    return Regex::is_valid(pattern) ? 1 : 0;
}

} // extern "C"
