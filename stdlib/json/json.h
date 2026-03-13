#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>

namespace sapphire {
namespace stdlib {
namespace JSON {

// Forward declaration
struct JSONValue;

// JSON Value Types
using JSONNull = std::nullptr_t;
using JSONBool = bool;
using JSONNumber = double;
using JSONString = std::string;
using JSONArray = std::vector<std::shared_ptr<JSONValue>>;
using JSONObject = std::map<std::string, std::shared_ptr<JSONValue>>;

// JSON Value variant
using JSONData = std::variant<JSONNull, JSONBool, JSONNumber, JSONString, JSONArray, JSONObject>;

// JSON Value structure
struct JSONValue {
    JSONData data;
    
    JSONValue() : data(nullptr) {}
    JSONValue(JSONNull val) : data(val) {}
    JSONValue(JSONBool val) : data(val) {}
    JSONValue(JSONNumber val) : data(val) {}
    JSONValue(const JSONString& val) : data(val) {}
    JSONValue(const JSONArray& val) : data(val) {}
    JSONValue(const JSONObject& val) : data(val) {}
    
    // Type checking
    bool is_null() const { return std::holds_alternative<JSONNull>(data); }
    bool is_bool() const { return std::holds_alternative<JSONBool>(data); }
    bool is_number() const { return std::holds_alternative<JSONNumber>(data); }
    bool is_string() const { return std::holds_alternative<JSONString>(data); }
    bool is_array() const { return std::holds_alternative<JSONArray>(data); }
    bool is_object() const { return std::holds_alternative<JSONObject>(data); }
    
    // Value access
    JSONBool as_bool() const { return std::get<JSONBool>(data); }
    JSONNumber as_number() const { return std::get<JSONNumber>(data); }
    const JSONString& as_string() const { return std::get<JSONString>(data); }
    const JSONArray& as_array() const { return std::get<JSONArray>(data); }
    const JSONObject& as_object() const { return std::get<JSONObject>(data); }
    
    // Mutable access
    JSONArray& as_array() { return std::get<JSONArray>(data); }
    JSONObject& as_object() { return std::get<JSONObject>(data); }
};

// JSON Parser
class JSONParser {
private:
    std::string text;
    size_t pos;
    
public:
    JSONParser(const std::string& json_text);
    
    std::shared_ptr<JSONValue> parse();
    std::string get_error() const { return error_message; }
    
private:
    std::string error_message;
    
    std::shared_ptr<JSONValue> parse_value();
    std::shared_ptr<JSONValue> parse_object();
    std::shared_ptr<JSONValue> parse_array();
    std::shared_ptr<JSONValue> parse_string();
    std::shared_ptr<JSONValue> parse_number();
    std::shared_ptr<JSONValue> parse_literal();
    
    void skip_whitespace();
    char peek() const;
    char advance();
    bool match(char expected);
    bool is_at_end() const;
    void set_error(const std::string& message);
};

// JSON Generator
class JSONGenerator {
public:
    static std::string stringify(const std::shared_ptr<JSONValue>& value, bool pretty = false, int indent = 0);
    
private:
    static std::string stringify_value(const std::shared_ptr<JSONValue>& value, bool pretty, int indent);
    static std::string stringify_string(const std::string& str);
    static std::string get_indent(int level);
};

// Convenience functions
std::shared_ptr<JSONValue> parse_json(const std::string& json_text);
std::string stringify_json(const std::shared_ptr<JSONValue>& value, bool pretty = false);

// Factory functions
std::shared_ptr<JSONValue> create_json_null();
std::shared_ptr<JSONValue> create_json_bool(bool value);
std::shared_ptr<JSONValue> create_json_number(double value);
std::shared_ptr<JSONValue> create_json_string(const std::string& value);
std::shared_ptr<JSONValue> create_json_array();
std::shared_ptr<JSONValue> create_json_object();

} // namespace JSON
} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // JSON parsing
    void* sapphire_json_parse(const char* json_text);
    const char* sapphire_json_stringify(void* json_value, bool pretty);
    void sapphire_json_destroy(void* json_value);
    
    // JSON creation
    void* sapphire_json_create_null();
    void* sapphire_json_create_bool(bool value);
    void* sapphire_json_create_number(double value);
    void* sapphire_json_create_string(const char* value);
    void* sapphire_json_create_array();
    void* sapphire_json_create_object();
    
    // Type checking
    bool sapphire_json_is_null(void* json_value);
    bool sapphire_json_is_bool(void* json_value);
    bool sapphire_json_is_number(void* json_value);
    bool sapphire_json_is_string(void* json_value);
    bool sapphire_json_is_array(void* json_value);
    bool sapphire_json_is_object(void* json_value);
    
    // Value access
    bool sapphire_json_get_bool(void* json_value);
    double sapphire_json_get_number(void* json_value);
    const char* sapphire_json_get_string(void* json_value);
    
    // Array operations
    int64_t sapphire_json_array_size(void* json_value);
    void* sapphire_json_array_get(void* json_value, int64_t index);
    void sapphire_json_array_push(void* json_value, void* element);
    
    // Object operations
    void* sapphire_json_object_get(void* json_value, const char* key);
    void sapphire_json_object_set(void* json_value, const char* key, void* element);
    bool sapphire_json_object_has(void* json_value, const char* key);
}