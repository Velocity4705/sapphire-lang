#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace sapphire {
namespace stdlib {
namespace Template {

// Template variable types
using TemplateValue = std::string;
using TemplateContext = std::unordered_map<std::string, TemplateValue>;

// Template engine class
class TemplateEngine {
public:
    TemplateEngine();
    ~TemplateEngine();
    
    // Template rendering
    std::string render(const std::string& template_str, const TemplateContext& context);
    std::string render_file(const std::string& filename, const TemplateContext& context);
    
    // Template management
    void set_template_dir(const std::string& dir);
    void cache_template(const std::string& name, const std::string& content);
    void clear_cache();
    
    // Configuration
    void set_delimiters(const std::string& open, const std::string& close);
    void enable_auto_escape(bool enabled);

private:
    std::string template_dir_;
    std::unordered_map<std::string, std::string> template_cache_;
    std::string open_delimiter_;
    std::string close_delimiter_;
    bool auto_escape_;
    
    std::string process_template(const std::string& template_str, const TemplateContext& context);
    std::string substitute_variables(const std::string& text, const TemplateContext& context);
    std::string process_conditionals(const std::string& text, const TemplateContext& context);
    std::string process_loops(const std::string& text, const TemplateContext& context);
    std::string html_escape(const std::string& text);
    std::string load_template_file(const std::string& filename);
};

} // namespace Template
} // namespace stdlib
} // namespace sapphire

// C API for Sapphire integration
extern "C" {
    void* sapphire_html_template_engine_create();
    void sapphire_html_template_engine_destroy(void* engine);
    char* sapphire_html_template_render(void* engine, const char* template_str, const char** keys, const char** values, int count);
    char* sapphire_html_template_render_file(void* engine, const char* filename, const char** keys, const char** values, int count);
    void sapphire_html_template_set_dir(void* engine, const char* dir);
    void sapphire_html_template_cache(void* engine, const char* name, const char* content);
    void sapphire_html_template_clear_cache(void* engine);
    void sapphire_html_template_free_string(char* str);
}