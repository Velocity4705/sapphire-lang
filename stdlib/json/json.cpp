#include "json.h"
#include <sstream>
#include <cctype>
#include <cstring>
#include <iomanip>

namespace sapphire {
namespace stdlib {
namespace JSON {

// JSONParser Implementation
JSONParser::JSONParser(const std::string& json_text) : text(json_text), pos(0) {}

std::shared_ptr<JSONValue> JSONParser::parse() {
    skip_whitespace();
    auto result = parse_value();
    skip_whitespace();
    
    if (!is_at_end()) {
        set_error("Unexpected characters after JSON value");
        return nullptr;
    }
    
    return result;
}

std::shared_ptr<JSONValue> JSONParser::parse_value() {
    skip_whitespace();
    
    if (is_at_end()) {
        set_error("Unexpected end of input");
        return nullptr;
    }
    
    char c = peek();
    
    switch (c) {
        case '{': return parse_object();
        case '[': return parse_array();
        case '"': return parse_string();
        case 't': case 'f': case 'n': return parse_literal();
        default:
            if (c == '-' || std::isdigit(c)) {
                return parse_number();
            }
            set_error("Unexpected character: " + std::string(1, c));
            return nullptr;
    }
}

std::shared_ptr<JSONValue> JSONParser::parse_object() {
    if (!match('{')) {
        set_error("Expected '{'");
        return nullptr;
    }
    
    auto obj = std::make_shared<JSONValue>(JSONObject{});
    auto& object = obj->as_object();
    
    skip_whitespace();
    
    // Empty object
    if (match('}')) {
        return obj;
    }
    
    while (true) {
        skip_whitespace();
        
        // Parse key
        if (peek() != '"') {
            set_error("Expected string key");
            return nullptr;
        }
        
        auto key_value = parse_string();
        if (!key_value) return nullptr;
        
        std::string key = key_value->as_string();
        
        skip_whitespace();
        
        // Expect colon
        if (!match(':')) {
            set_error("Expected ':'");
            return nullptr;
        }
        
        // Parse value
        auto value = parse_value();
        if (!value) return nullptr;
        
        object[key] = value;
        
        skip_whitespace();
        
        if (match('}')) {
            break;
        } else if (match(',')) {
            continue;
        } else {
            set_error("Expected ',' or '}'");
            return nullptr;
        }
    }
    
    return obj;
}

std::shared_ptr<JSONValue> JSONParser::parse_array() {
    if (!match('[')) {
        set_error("Expected '['");
        return nullptr;
    }
    
    auto arr = std::make_shared<JSONValue>(JSONArray{});
    auto& array = arr->as_array();
    
    skip_whitespace();
    
    // Empty array
    if (match(']')) {
        return arr;
    }
    
    while (true) {
        auto value = parse_value();
        if (!value) return nullptr;
        
        array.push_back(value);
        
        skip_whitespace();
        
        if (match(']')) {
            break;
        } else if (match(',')) {
            continue;
        } else {
            set_error("Expected ',' or ']'");
            return nullptr;
        }
    }
    
    return arr;
}

std::shared_ptr<JSONValue> JSONParser::parse_string() {
    if (!match('"')) {
        set_error("Expected '\"'");
        return nullptr;
    }
    
    std::string result;
    
    while (!is_at_end() && peek() != '"') {
        char c = advance();
        
        if (c == '\\') {
            if (is_at_end()) {
                set_error("Unterminated string escape");
                return nullptr;
            }
            
            char escaped = advance();
            switch (escaped) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    // Unicode escape (simplified - just add the literal for now)
                    result += "\\u";
                    for (int i = 0; i < 4 && !is_at_end(); i++) {
                        result += advance();
                    }
                    break;
                }
                default:
                    set_error("Invalid escape sequence: \\" + std::string(1, escaped));
                    return nullptr;
            }
        } else {
            result += c;
        }
    }
    
    if (!match('"')) {
        set_error("Unterminated string");
        return nullptr;
    }
    
    return std::make_shared<JSONValue>(result);
}

std::shared_ptr<JSONValue> JSONParser::parse_number() {
    size_t start = pos;
    
    // Optional minus
    if (peek() == '-') {
        advance();
    }
    
    // Integer part
    if (peek() == '0') {
        advance();
    } else if (std::isdigit(peek())) {
        while (!is_at_end() && std::isdigit(peek())) {
            advance();
        }
    } else {
        set_error("Invalid number");
        return nullptr;
    }
    
    // Fractional part
    if (!is_at_end() && peek() == '.') {
        advance();
        if (!std::isdigit(peek())) {
            set_error("Invalid number: expected digit after '.'");
            return nullptr;
        }
        while (!is_at_end() && std::isdigit(peek())) {
            advance();
        }
    }
    
    // Exponent part
    if (!is_at_end() && (peek() == 'e' || peek() == 'E')) {
        advance();
        if (!is_at_end() && (peek() == '+' || peek() == '-')) {
            advance();
        }
        if (!std::isdigit(peek())) {
            set_error("Invalid number: expected digit in exponent");
            return nullptr;
        }
        while (!is_at_end() && std::isdigit(peek())) {
            advance();
        }
    }
    
    std::string number_str = text.substr(start, pos - start);
    double value = std::stod(number_str);
    
    return std::make_shared<JSONValue>(value);
}

std::shared_ptr<JSONValue> JSONParser::parse_literal() {
    if (text.substr(pos, 4) == "true") {
        pos += 4;
        return std::make_shared<JSONValue>(true);
    } else if (text.substr(pos, 5) == "false") {
        pos += 5;
        return std::make_shared<JSONValue>(false);
    } else if (text.substr(pos, 4) == "null") {
        pos += 4;
        return std::make_shared<JSONValue>(nullptr);
    } else {
        set_error("Invalid literal");
        return nullptr;
    }
}

void JSONParser::skip_whitespace() {
    while (!is_at_end() && std::isspace(peek())) {
        advance();
    }
}

char JSONParser::peek() const {
    return is_at_end() ? '\0' : text[pos];
}

char JSONParser::advance() {
    return is_at_end() ? '\0' : text[pos++];
}

bool JSONParser::match(char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

bool JSONParser::is_at_end() const {
    return pos >= text.length();
}

void JSONParser::set_error(const std::string& message) {
    error_message = message + " at position " + std::to_string(pos);
}

// JSONGenerator Implementation
std::string JSONGenerator::stringify(const std::shared_ptr<JSONValue>& value, bool pretty, int indent) {
    if (!value) return "null";
    return stringify_value(value, pretty, indent);
}

std::string JSONGenerator::stringify_value(const std::shared_ptr<JSONValue>& value, bool pretty, int indent) {
    if (value->is_null()) {
        return "null";
    } else if (value->is_bool()) {
        return value->as_bool() ? "true" : "false";
    } else if (value->is_number()) {
        double num = value->as_number();
        if (num == static_cast<int64_t>(num)) {
            return std::to_string(static_cast<int64_t>(num));
        } else {
            return std::to_string(num);
        }
    } else if (value->is_string()) {
        return stringify_string(value->as_string());
    } else if (value->is_array()) {
        const auto& array = value->as_array();
        std::ostringstream oss;
        oss << "[";
        
        for (size_t i = 0; i < array.size(); i++) {
            if (i > 0) oss << ",";
            if (pretty) {
                oss << "\n" << get_indent(indent + 1);
            }
            oss << stringify_value(array[i], pretty, indent + 1);
        }
        
        if (pretty && !array.empty()) {
            oss << "\n" << get_indent(indent);
        }
        oss << "]";
        return oss.str();
    } else if (value->is_object()) {
        const auto& object = value->as_object();
        std::ostringstream oss;
        oss << "{";
        
        bool first = true;
        for (const auto& [key, val] : object) {
            if (!first) oss << ",";
            if (pretty) {
                oss << "\n" << get_indent(indent + 1);
            }
            oss << stringify_string(key) << ":";
            if (pretty) oss << " ";
            oss << stringify_value(val, pretty, indent + 1);
            first = false;
        }
        
        if (pretty && !object.empty()) {
            oss << "\n" << get_indent(indent);
        }
        oss << "}";
        return oss.str();
    }
    
    return "null";
}

std::string JSONGenerator::stringify_string(const std::string& str) {
    std::ostringstream oss;
    oss << "\"";
    
    for (char c : str) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 32) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    oss << c;
                }
                break;
        }
    }
    
    oss << "\"";
    return oss.str();
}

std::string JSONGenerator::get_indent(int level) {
    return std::string(level * 2, ' ');
}

// Convenience functions
std::shared_ptr<JSONValue> parse_json(const std::string& json_text) {
    JSONParser parser(json_text);
    return parser.parse();
}

std::string stringify_json(const std::shared_ptr<JSONValue>& value, bool pretty) {
    return JSONGenerator::stringify(value, pretty);
}

// Factory functions
std::shared_ptr<JSONValue> create_json_null() {
    return std::make_shared<JSONValue>(nullptr);
}

std::shared_ptr<JSONValue> create_json_bool(bool value) {
    return std::make_shared<JSONValue>(value);
}

std::shared_ptr<JSONValue> create_json_number(double value) {
    return std::make_shared<JSONValue>(value);
}

std::shared_ptr<JSONValue> create_json_string(const std::string& value) {
    return std::make_shared<JSONValue>(value);
}

std::shared_ptr<JSONValue> create_json_array() {
    return std::make_shared<JSONValue>(JSONArray{});
}

std::shared_ptr<JSONValue> create_json_object() {
    return std::make_shared<JSONValue>(JSONObject{});
}

} // namespace JSON
} // namespace stdlib
} // namespace sapphire

// C API Implementation
extern "C" {
    static thread_local char g_json_string_buffer[16384];
    
    void* sapphire_json_parse(const char* json_text) {
        if (!json_text) return nullptr;
        
        auto result = sapphire::stdlib::JSON::parse_json(json_text);
        if (result) {
            return new std::shared_ptr<sapphire::stdlib::JSON::JSONValue>(result);
        }
        return nullptr;
    }
    
    const char* sapphire_json_stringify(void* json_value, bool pretty) {
        if (!json_value) return nullptr;
        
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        std::string result = sapphire::stdlib::JSON::stringify_json(*value_ptr, pretty);
        
        strncpy(g_json_string_buffer, result.c_str(), sizeof(g_json_string_buffer) - 1);
        g_json_string_buffer[sizeof(g_json_string_buffer) - 1] = '\0';
        
        return g_json_string_buffer;
    }
    
    void sapphire_json_destroy(void* json_value) {
        if (json_value) {
            delete (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        }
    }
    
    // JSON creation functions
    void* sapphire_json_create_null() {
        auto result = sapphire::stdlib::JSON::create_json_null();
        return new std::shared_ptr<sapphire::stdlib::JSON::JSONValue>(result);
    }
    
    void* sapphire_json_create_bool(bool value) {
        auto result = sapphire::stdlib::JSON::create_json_bool(value);
        return new std::shared_ptr<sapphire::stdlib::JSON::JSONValue>(result);
    }
    
    void* sapphire_json_create_number(double value) {
        auto result = sapphire::stdlib::JSON::create_json_number(value);
        return new std::shared_ptr<sapphire::stdlib::JSON::JSONValue>(result);
    }
    
    void* sapphire_json_create_string(const char* value) {
        if (!value) return nullptr;
        auto result = sapphire::stdlib::JSON::create_json_string(value);
        return new std::shared_ptr<sapphire::stdlib::JSON::JSONValue>(result);
    }
    
    void* sapphire_json_create_array() {
        auto result = sapphire::stdlib::JSON::create_json_array();
        return new std::shared_ptr<sapphire::stdlib::JSON::JSONValue>(result);
    }
    
    void* sapphire_json_create_object() {
        auto result = sapphire::stdlib::JSON::create_json_object();
        return new std::shared_ptr<sapphire::stdlib::JSON::JSONValue>(result);
    }
    
    // Type checking functions
    bool sapphire_json_is_null(void* json_value) {
        if (!json_value) return true;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        return (*value_ptr)->is_null();
    }
    
    bool sapphire_json_is_bool(void* json_value) {
        if (!json_value) return false;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        return (*value_ptr)->is_bool();
    }
    
    bool sapphire_json_is_number(void* json_value) {
        if (!json_value) return false;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        return (*value_ptr)->is_number();
    }
    
    bool sapphire_json_is_string(void* json_value) {
        if (!json_value) return false;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        return (*value_ptr)->is_string();
    }
    
    bool sapphire_json_is_array(void* json_value) {
        if (!json_value) return false;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        return (*value_ptr)->is_array();
    }
    
    bool sapphire_json_is_object(void* json_value) {
        if (!json_value) return false;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        return (*value_ptr)->is_object();
    }
    
    // Value access functions
    bool sapphire_json_get_bool(void* json_value) {
        if (!json_value) return false;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        return (*value_ptr)->is_bool() ? (*value_ptr)->as_bool() : false;
    }
    
    double sapphire_json_get_number(void* json_value) {
        if (!json_value) return 0.0;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        return (*value_ptr)->is_number() ? (*value_ptr)->as_number() : 0.0;
    }
    
    const char* sapphire_json_get_string(void* json_value) {
        if (!json_value) return nullptr;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        if ((*value_ptr)->is_string()) {
            const std::string& str = (*value_ptr)->as_string();
            strncpy(g_json_string_buffer, str.c_str(), sizeof(g_json_string_buffer) - 1);
            g_json_string_buffer[sizeof(g_json_string_buffer) - 1] = '\0';
            return g_json_string_buffer;
        }
        return nullptr;
    }
    
    // Array operations
    int64_t sapphire_json_array_size(void* json_value) {
        if (!json_value) return 0;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        return (*value_ptr)->is_array() ? (int64_t)(*value_ptr)->as_array().size() : 0;
    }
    
    void* sapphire_json_array_get(void* json_value, int64_t index) {
        if (!json_value || index < 0) return nullptr;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        if ((*value_ptr)->is_array()) {
            const auto& array = (*value_ptr)->as_array();
            if ((size_t)index < array.size()) {
                return new std::shared_ptr<sapphire::stdlib::JSON::JSONValue>(array[index]);
            }
        }
        return nullptr;
    }
    
    void sapphire_json_array_push(void* json_value, void* element) {
        if (!json_value || !element) return;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        auto* element_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)element;
        if ((*value_ptr)->is_array()) {
            (*value_ptr)->as_array().push_back(*element_ptr);
        }
    }
    
    // Object operations
    void* sapphire_json_object_get(void* json_value, const char* key) {
        if (!json_value || !key) return nullptr;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        if ((*value_ptr)->is_object()) {
            const auto& object = (*value_ptr)->as_object();
            auto it = object.find(key);
            if (it != object.end()) {
                return new std::shared_ptr<sapphire::stdlib::JSON::JSONValue>(it->second);
            }
        }
        return nullptr;
    }
    
    void sapphire_json_object_set(void* json_value, const char* key, void* element) {
        if (!json_value || !key || !element) return;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        auto* element_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)element;
        if ((*value_ptr)->is_object()) {
            (*value_ptr)->as_object()[key] = *element_ptr;
        }
    }
    
    bool sapphire_json_object_has(void* json_value, const char* key) {
        if (!json_value || !key) return false;
        auto* value_ptr = (std::shared_ptr<sapphire::stdlib::JSON::JSONValue>*)json_value;
        if ((*value_ptr)->is_object()) {
            const auto& object = (*value_ptr)->as_object();
            return object.find(key) != object.end();
        }
        return false;
    }
}