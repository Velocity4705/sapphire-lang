#include "template.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>
#include <cstring>

namespace sapphire {
namespace stdlib {
namespace Template {

TemplateEngine::TemplateEngine() 
    : template_dir_("./templates"), open_delimiter_("{{"), close_delimiter_("}}"), auto_escape_(true) {
}

TemplateEngine::~TemplateEngine() {
}

std::string TemplateEngine::render(const std::string& template_str, const TemplateContext& context) {
    return process_template(template_str, context);
}

std::string TemplateEngine::render_file(const std::string& filename, const TemplateContext& context) {
    std::string template_content;
    
    // Check cache first
    auto cache_it = template_cache_.find(filename);
    if (cache_it != template_cache_.end()) {
        template_content = cache_it->second;
    } else {
        template_content = load_template_file(filename);
        template_cache_[filename] = template_content;
    }
    
    return process_template(template_content, context);
}

void TemplateEngine::set_template_dir(const std::string& dir) {
    template_dir_ = dir;
}

void TemplateEngine::cache_template(const std::string& name, const std::string& content) {
    template_cache_[name] = content;
}

void TemplateEngine::clear_cache() {
    template_cache_.clear();
}

void TemplateEngine::set_delimiters(const std::string& open, const std::string& close) {
    open_delimiter_ = open;
    close_delimiter_ = close;
}

void TemplateEngine::enable_auto_escape(bool enabled) {
    auto_escape_ = enabled;
}
std::string TemplateEngine::process_template(const std::string& template_str, const TemplateContext& context) {
    std::string result = template_str;
    
    // Process in order: loops, conditionals, then variables
    result = process_loops(result, context);
    result = process_conditionals(result, context);
    result = substitute_variables(result, context);
    
    return result;
}

std::string TemplateEngine::substitute_variables(const std::string& text, const TemplateContext& context) {
    std::string result = text;
    
    // Simple variable substitution: {{variable}}
    for (const auto& pair : context) {
        std::string placeholder = open_delimiter_ + pair.first + close_delimiter_;
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            std::string replacement = auto_escape_ ? html_escape(pair.second) : pair.second;
            result.replace(pos, placeholder.length(), replacement);
            pos += replacement.length();
        }
    }
    
    return result;
}

std::string TemplateEngine::process_conditionals(const std::string& text, const TemplateContext& context) {
    std::string result = text;
    
    // Simple if/endif processing without regex
    for (const auto& pair : context) {
        std::string if_start = open_delimiter_ + "if " + pair.first + close_delimiter_;
        std::string if_end = open_delimiter_ + "endif" + close_delimiter_;
        
        size_t start_pos = result.find(if_start);
        if (start_pos != std::string::npos) {
            size_t end_pos = result.find(if_end, start_pos);
            if (end_pos != std::string::npos) {
                size_t content_start = start_pos + if_start.length();
                std::string content = result.substr(content_start, end_pos - content_start);
                
                std::string replacement;
                if (!pair.second.empty() && pair.second != "false" && pair.second != "0") {
                    replacement = content;
                }
                
                result.replace(start_pos, end_pos + if_end.length() - start_pos, replacement);
            }
        }
    }
    
    return result;
}

std::string TemplateEngine::process_loops(const std::string& text, const TemplateContext& context) {
    // Simplified loop processing - for now just return as-is
    // In a full implementation, this would handle {% for item in items %} syntax
    (void)context; // Suppress unused parameter warning
    return text;
}

std::string TemplateEngine::html_escape(const std::string& text) {
    std::string result = text;
    
    // Basic HTML escaping
    std::regex amp_regex("&");
    result = std::regex_replace(result, amp_regex, "&amp;");
    
    std::regex lt_regex("<");
    result = std::regex_replace(result, lt_regex, "&lt;");
    
    std::regex gt_regex(">");
    result = std::regex_replace(result, gt_regex, "&gt;");
    
    std::regex quot_regex("\"");
    result = std::regex_replace(result, quot_regex, "&quot;");
    
    std::regex apos_regex("'");
    result = std::regex_replace(result, apos_regex, "&#x27;");
    
    return result;
}

std::string TemplateEngine::load_template_file(const std::string& filename) {
    std::string full_path = template_dir_ + "/" + filename;
    std::ifstream file(full_path);
    
    if (!file.is_open()) {
        return ""; // Template not found
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace Template
} // namespace stdlib
} // namespace sapphire

// C API Implementation
extern "C" {
    using namespace sapphire::stdlib::Template;
    
    void* sapphire_html_template_engine_create() {
        return new TemplateEngine();
    }
    
    void sapphire_html_template_engine_destroy(void* engine) {
        delete static_cast<TemplateEngine*>(engine);
    }
    
    char* sapphire_html_template_render(void* engine, const char* template_str, const char** keys, const char** values, int count) {
        TemplateContext context;
        for (int i = 0; i < count; ++i) {
            context[keys[i]] = values[i];
        }
        
        std::string result = static_cast<TemplateEngine*>(engine)->render(std::string(template_str), context);
        char* c_result = new char[result.length() + 1];
        strcpy(c_result, result.c_str());
        return c_result;
    }
    
    char* sapphire_html_template_render_file(void* engine, const char* filename, const char** keys, const char** values, int count) {
        TemplateContext context;
        for (int i = 0; i < count; ++i) {
            context[keys[i]] = values[i];
        }
        
        std::string result = static_cast<TemplateEngine*>(engine)->render_file(std::string(filename), context);
        char* c_result = new char[result.length() + 1];
        strcpy(c_result, result.c_str());
        return c_result;
    }
    
    void sapphire_html_template_set_dir(void* engine, const char* dir) {
        static_cast<TemplateEngine*>(engine)->set_template_dir(std::string(dir));
    }
    
    void sapphire_html_template_cache(void* engine, const char* name, const char* content) {
        static_cast<TemplateEngine*>(engine)->cache_template(std::string(name), std::string(content));
    }
    
    void sapphire_html_template_clear_cache(void* engine) {
        static_cast<TemplateEngine*>(engine)->clear_cache();
    }
    
    void sapphire_html_template_free_string(char* str) {
        delete[] str;
    }
}