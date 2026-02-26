#ifndef SAPPHIRE_STDLIB_JSON_H
#define SAPPHIRE_STDLIB_JSON_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>

namespace sapphire {
namespace json {

enum class Type {
    Null,
    Bool,
    Number,
    String,
    Array,
    Object
};

class Value {
private:
    using ArrayType = std::vector<Value>;
    using ObjectType = std::map<std::string, Value>;
    using ValueType = std::variant<
        std::nullptr_t,
        bool,
        double,
        std::string,
        ArrayType,
        ObjectType
    >;
    
    ValueType data_;
    
public:
    // Constructors
    Value();
    Value(std::nullptr_t);
    Value(bool b);
    Value(int n);
    Value(double n);
    Value(const char* s);
    Value(const std::string& s);
    Value(const ArrayType& arr);
    Value(const ObjectType& obj);
    
    // Type checking
    Type type() const;
    bool is_null() const;
    bool is_bool() const;
    bool is_number() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;
    
    // Accessors
    bool as_bool() const;
    double as_number() const;
    std::string as_string() const;
    const ArrayType& as_array() const;
    const ObjectType& as_object() const;
    
    // Array/Object access
    Value& operator[](size_t index);
    const Value& operator[](size_t index) const;
    Value& operator[](const std::string& key);
    const Value& operator[](const std::string& key) const;
    
    // Array operations
    void push(const Value& val);
    size_t size() const;
    
    // Object operations
    bool has(const std::string& key) const;
    void set(const std::string& key, const Value& val);
    
    // Parsing and serialization
    static Value parse(const std::string& json);
    std::string serialize(bool pretty = false, int indent = 0) const;
};

} // namespace json
} // namespace sapphire

#endif // SAPPHIRE_STDLIB_JSON_H
