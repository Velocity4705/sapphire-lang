#include "json.h"
#include <sstream>
#include <stdexcept>
#include <cctype>

namespace sapphire {
namespace json {

// Constructors
Value::Value() : data_(nullptr) {}
Value::Value(std::nullptr_t) : data_(nullptr) {}
Value::Value(bool b) : data_(b) {}
Value::Value(int n) : data_(static_cast<double>(n)) {}
Value::Value(double n) : data_(n) {}
Value::Value(const char* s) : data_(std::string(s)) {}
Value::Value(const std::string& s) : data_(s) {}
Value::Value(const ArrayType& arr) : data_(arr) {}
Value::Value(const ObjectType& obj) : data_(obj) {}

// Type checking
Type Value::type() const {
    if (std::holds_alternative<std::nullptr_t>(data_)) return Type::Null;
    if (std::holds_alternative<bool>(data_)) return Type::Bool;
    if (std::holds_alternative<double>(data_)) return Type::Number;
    if (std::holds_alternative<std::string>(data_)) return Type::String;
    if (std::holds_alternative<ArrayType>(data_)) return Type::Array;
    if (std::holds_alternative<ObjectType>(data_)) return Type::Object;
    return Type::Null;
}

bool Value::is_null() const { return type() == Type::Null; }
bool Value::is_bool() const { return type() == Type::Bool; }
bool Value::is_number() const { return type() == Type::Number; }
bool Value::is_string() const { return type() == Type::String; }
bool Value::is_array() const { return type() == Type::Array; }
bool Value::is_object() const { return type() == Type::Object; }

// Accessors
bool Value::as_bool() const {
    if (!is_bool()) throw std::runtime_error("Value is not a boolean");
    return std::get<bool>(data_);
}

double Value::as_number() const {
    if (!is_number()) throw std::runtime_error("Value is not a number");
    return std::get<double>(data_);
}

std::string Value::as_string() const {
    if (!is_string()) throw std::runtime_error("Value is not a string");
    return std::get<std::string>(data_);
}

const Value::ArrayType& Value::as_array() const {
    if (!is_array()) throw std::runtime_error("Value is not an array");
    return std::get<ArrayType>(data_);
}

const Value::ObjectType& Value::as_object() const {
    if (!is_object()) throw std::runtime_error("Value is not an object");
    return std::get<ObjectType>(data_);
}

// Array/Object access
Value& Value::operator[](size_t index) {
    if (!is_array()) {
        data_ = ArrayType();
    }
    auto& arr = std::get<ArrayType>(data_);
    if (index >= arr.size()) {
        arr.resize(index + 1);
    }
    return arr[index];
}

const Value& Value::operator[](size_t index) const {
    return const_cast<Value*>(this)->operator[](index);
}

Value& Value::operator[](const std::string& key) {
    if (!is_object()) {
        data_ = ObjectType();
    }
    return std::get<ObjectType>(data_)[key];
}

const Value& Value::operator[](const std::string& key) const {
    return const_cast<Value*>(this)->operator[](key);
}

// Array operations
void Value::push(const Value& val) {
    if (!is_array()) {
        data_ = ArrayType();
    }
    std::get<ArrayType>(data_).push_back(val);
}

size_t Value::size() const {
    if (is_array()) return std::get<ArrayType>(data_).size();
    if (is_object()) return std::get<ObjectType>(data_).size();
    return 0;
}

// Object operations
bool Value::has(const std::string& key) const {
    if (!is_object()) return false;
    const auto& obj = std::get<ObjectType>(data_);
    return obj.find(key) != obj.end();
}

void Value::set(const std::string& key, const Value& val) {
    if (!is_object()) {
        data_ = ObjectType();
    }
    std::get<ObjectType>(data_)[key] = val;
}

// Simple JSON parser
static void skip_whitespace(const std::string& json, size_t& pos) {
    while (pos < json.length() && std::isspace(json[pos])) {
        pos++;
    }
}

static Value parse_value(const std::string& json, size_t& pos);

static Value parse_string(const std::string& json, size_t& pos) {
    if (json[pos] != '"') throw std::runtime_error("Expected '\"'");
    pos++;
    
    std::string result;
    while (pos < json.length() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.length()) {
            pos++;
            switch (json[pos]) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                default: result += json[pos];
            }
        } else {
            result += json[pos];
        }
        pos++;
    }
    
    if (pos >= json.length()) throw std::runtime_error("Unterminated string");
    pos++; // Skip closing quote
    
    return Value(result);
}

static Value parse_number(const std::string& json, size_t& pos) {
    size_t start = pos;
    if (json[pos] == '-') pos++;
    
    while (pos < json.length() && (std::isdigit(json[pos]) || json[pos] == '.')) {
        pos++;
    }
    
    std::string num_str = json.substr(start, pos - start);
    return Value(std::stod(num_str));
}

static Value parse_array(const std::string& json, size_t& pos) {
    if (json[pos] != '[') throw std::runtime_error("Expected '['");
    pos++;
    
    std::vector<Value> arr;
    skip_whitespace(json, pos);
    
    if (json[pos] == ']') {
        pos++;
        return Value(arr);
    }
    
    while (true) {
        skip_whitespace(json, pos);
        arr.push_back(parse_value(json, pos));
        skip_whitespace(json, pos);
        
        if (json[pos] == ']') {
            pos++;
            break;
        }
        
        if (json[pos] != ',') throw std::runtime_error("Expected ',' or ']'");
        pos++;
    }
    
    return Value(arr);
}

static Value parse_object(const std::string& json, size_t& pos) {
    if (json[pos] != '{') throw std::runtime_error("Expected '{'");
    pos++;
    
    std::map<std::string, Value> obj;
    skip_whitespace(json, pos);
    
    if (json[pos] == '}') {
        pos++;
        return Value(obj);
    }
    
    while (true) {
        skip_whitespace(json, pos);
        
        if (json[pos] != '"') throw std::runtime_error("Expected string key");
        std::string key = parse_string(json, pos).as_string();
        
        skip_whitespace(json, pos);
        if (json[pos] != ':') throw std::runtime_error("Expected ':'");
        pos++;
        
        skip_whitespace(json, pos);
        obj[key] = parse_value(json, pos);
        
        skip_whitespace(json, pos);
        
        if (json[pos] == '}') {
            pos++;
            break;
        }
        
        if (json[pos] != ',') throw std::runtime_error("Expected ',' or '}'");
        pos++;
    }
    
    return Value(obj);
}

static Value parse_value(const std::string& json, size_t& pos) {
    skip_whitespace(json, pos);
    
    if (pos >= json.length()) throw std::runtime_error("Unexpected end of JSON");
    
    char c = json[pos];
    
    if (c == '"') {
        return parse_string(json, pos);
    } else if (c == '{') {
        return parse_object(json, pos);
    } else if (c == '[') {
        return parse_array(json, pos);
    } else if (c == 't' && json.substr(pos, 4) == "true") {
        pos += 4;
        return Value(true);
    } else if (c == 'f' && json.substr(pos, 5) == "false") {
        pos += 5;
        return Value(false);
    } else if (c == 'n' && json.substr(pos, 4) == "null") {
        pos += 4;
        return Value(nullptr);
    } else if (c == '-' || std::isdigit(c)) {
        return parse_number(json, pos);
    }
    
    throw std::runtime_error("Invalid JSON value");
}

Value Value::parse(const std::string& json) {
    size_t pos = 0;
    return parse_value(json, pos);
}

std::string Value::serialize(bool pretty, int indent) const {
    std::ostringstream ss;
    std::string indent_str = pretty ? std::string(indent * 2, ' ') : "";
    std::string newline = pretty ? "\n" : "";
    
    switch (type()) {
        case Type::Null:
            ss << "null";
            break;
        case Type::Bool:
            ss << (as_bool() ? "true" : "false");
            break;
        case Type::Number:
            ss << as_number();
            break;
        case Type::String:
            ss << "\"" << as_string() << "\"";
            break;
        case Type::Array: {
            ss << "[" << newline;
            const auto& arr = as_array();
            for (size_t i = 0; i < arr.size(); i++) {
                if (pretty) ss << std::string((indent + 1) * 2, ' ');
                ss << arr[i].serialize(pretty, indent + 1);
                if (i < arr.size() - 1) ss << ",";
                ss << newline;
            }
            if (pretty) ss << indent_str;
            ss << "]";
            break;
        }
        case Type::Object: {
            ss << "{" << newline;
            const auto& obj = as_object();
            size_t i = 0;
            for (const auto& [key, val] : obj) {
                if (pretty) ss << std::string((indent + 1) * 2, ' ');
                ss << "\"" << key << "\":" << (pretty ? " " : "");
                ss << val.serialize(pretty, indent + 1);
                if (i < obj.size() - 1) ss << ",";
                ss << newline;
                i++;
            }
            if (pretty) ss << indent_str;
            ss << "}";
            break;
        }
    }
    
    return ss.str();
}

} // namespace json
} // namespace sapphire
