#ifndef SAPPHIRE_TYPE_H
#define SAPPHIRE_TYPE_H

#include <string>
#include <memory>
#include <vector>

namespace sapphire {

// Forward declarations
class TypeVisitor;

// Type kinds
enum class TypeKind {
    INT,
    FLOAT,
    STRING,
    BOOL,
    NONE,
    LIST,
    DICT,
    TUPLE,
    FUNCTION,
    GENERIC,
    TYPE_VAR,
    UNION,
    OPTIONAL
};

// Base Type class
class Type {
protected:
    TypeKind kind;
    
public:
    explicit Type(TypeKind k) : kind(k) {}
    virtual ~Type() = default;
    
    TypeKind getKind() const { return kind; }
    virtual std::string toString() const = 0;
    virtual bool equals(const Type& other) const = 0;
    virtual std::shared_ptr<Type> clone() const = 0;
    
    // Type checking helpers
    bool isInt() const { return kind == TypeKind::INT; }
    bool isFloat() const { return kind == TypeKind::FLOAT; }
    bool isString() const { return kind == TypeKind::STRING; }
    bool isBool() const { return kind == TypeKind::BOOL; }
    bool isNone() const { return kind == TypeKind::NONE; }
    bool isList() const { return kind == TypeKind::LIST; }
    bool isDict() const { return kind == TypeKind::DICT; }
    bool isFunction() const { return kind == TypeKind::FUNCTION; }
    bool isTypeVar() const { return kind == TypeKind::TYPE_VAR; }
    bool isGeneric() const { return kind == TypeKind::GENERIC; }
    bool isOptional() const { return kind == TypeKind::OPTIONAL; }
    
    // Numeric type check
    bool isNumeric() const { return isInt() || isFloat(); }
};

// Primitive types
class IntType : public Type {
public:
    IntType() : Type(TypeKind::INT) {}
    std::string toString() const override { return "int"; }
    bool equals(const Type& other) const override { return other.isInt(); }
    std::shared_ptr<Type> clone() const override { 
        return std::make_shared<IntType>(); 
    }
};

class FloatType : public Type {
public:
    FloatType() : Type(TypeKind::FLOAT) {}
    std::string toString() const override { return "float"; }
    bool equals(const Type& other) const override { return other.isFloat(); }
    std::shared_ptr<Type> clone() const override { 
        return std::make_shared<FloatType>(); 
    }
};

class StringType : public Type {
public:
    StringType() : Type(TypeKind::STRING) {}
    std::string toString() const override { return "string"; }
    bool equals(const Type& other) const override { return other.isString(); }
    std::shared_ptr<Type> clone() const override { 
        return std::make_shared<StringType>(); 
    }
};

class BoolType : public Type {
public:
    BoolType() : Type(TypeKind::BOOL) {}
    std::string toString() const override { return "bool"; }
    bool equals(const Type& other) const override { return other.isBool(); }
    std::shared_ptr<Type> clone() const override { 
        return std::make_shared<BoolType>(); 
    }
};

class NoneType : public Type {
public:
    NoneType() : Type(TypeKind::NONE) {}
    std::string toString() const override { return "none"; }
    bool equals(const Type& other) const override { return other.isNone(); }
    std::shared_ptr<Type> clone() const override { 
        return std::make_shared<NoneType>(); 
    }
};

// Type variable for inference
class TypeVariable : public Type {
private:
    static int next_id;
    int id;
    std::string name;
    
public:
    TypeVariable() : Type(TypeKind::TYPE_VAR), id(next_id++), name("T" + std::to_string(id)) {}
    explicit TypeVariable(const std::string& n) : Type(TypeKind::TYPE_VAR), id(next_id++), name(n) {}
    
    int getId() const { return id; }
    std::string getName() const { return name; }
    
    std::string toString() const override { return name; }
    bool equals(const Type& other) const override {
        if (!other.isTypeVar()) return false;
        return id == static_cast<const TypeVariable&>(other).id;
    }
    std::shared_ptr<Type> clone() const override {
        return std::make_shared<TypeVariable>(name);
    }
};

// List type
class ListType : public Type {
private:
    std::shared_ptr<Type> element_type;
    
public:
    explicit ListType(std::shared_ptr<Type> elem) 
        : Type(TypeKind::LIST), element_type(elem) {}
    
    std::shared_ptr<Type> getElementType() const { return element_type; }
    
    std::string toString() const override {
        return "List<" + element_type->toString() + ">";
    }
    
    bool equals(const Type& other) const override {
        if (!other.isList()) return false;
        const auto& other_list = static_cast<const ListType&>(other);
        return element_type->equals(*other_list.element_type);
    }
    
    std::shared_ptr<Type> clone() const override {
        return std::make_shared<ListType>(element_type->clone());
    }
};

// Function type
class FunctionType : public Type {
private:
    std::vector<std::shared_ptr<Type>> param_types;
    std::shared_ptr<Type> return_type;
    
public:
    FunctionType(std::vector<std::shared_ptr<Type>> params, std::shared_ptr<Type> ret)
        : Type(TypeKind::FUNCTION), param_types(std::move(params)), return_type(ret) {}
    
    const std::vector<std::shared_ptr<Type>>& getParamTypes() const { return param_types; }
    std::shared_ptr<Type> getReturnType() const { return return_type; }
    
    std::string toString() const override {
        std::string result = "fn(";
        for (size_t i = 0; i < param_types.size(); i++) {
            if (i > 0) result += ", ";
            result += param_types[i]->toString();
        }
        result += ") -> " + return_type->toString();
        return result;
    }
    
    bool equals(const Type& other) const override {
        if (!other.isFunction()) return false;
        const auto& other_fn = static_cast<const FunctionType&>(other);
        
        if (param_types.size() != other_fn.param_types.size()) return false;
        
        for (size_t i = 0; i < param_types.size(); i++) {
            if (!param_types[i]->equals(*other_fn.param_types[i])) return false;
        }
        
        return return_type->equals(*other_fn.return_type);
    }
    
    std::shared_ptr<Type> clone() const override {
        std::vector<std::shared_ptr<Type>> cloned_params;
        for (const auto& p : param_types) {
            cloned_params.push_back(p->clone());
        }
        return std::make_shared<FunctionType>(cloned_params, return_type->clone());
    }
};

// Optional type (T?)
class OptionalType : public Type {
private:
    std::shared_ptr<Type> inner_type;
    
public:
    explicit OptionalType(std::shared_ptr<Type> inner)
        : Type(TypeKind::OPTIONAL), inner_type(inner) {}
    
    std::shared_ptr<Type> getInnerType() const { return inner_type; }
    
    std::string toString() const override {
        return inner_type->toString() + "?";
    }
    
    bool equals(const Type& other) const override {
        if (!other.isOptional()) return false;
        const auto& other_opt = static_cast<const OptionalType&>(other);
        return inner_type->equals(*other_opt.inner_type);
    }
    
    std::shared_ptr<Type> clone() const override {
        return std::make_shared<OptionalType>(inner_type->clone());
    }
};

} // namespace sapphire

#endif // SAPPHIRE_TYPE_H
