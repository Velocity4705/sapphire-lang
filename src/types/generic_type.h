#ifndef SAPPHIRE_GENERIC_TYPE_H
#define SAPPHIRE_GENERIC_TYPE_H

#include "type.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace sapphire {

// Type parameter (e.g., T in List<T>)
class TypeParameter {
private:
    std::string name;
    std::vector<std::string> constraints; // Trait bounds
    
public:
    explicit TypeParameter(const std::string& n) : name(n) {}
    TypeParameter(const std::string& n, const std::vector<std::string>& c)
        : name(n), constraints(c) {}
    
    std::string getName() const { return name; }
    const std::vector<std::string>& getConstraints() const { return constraints; }
    
    void addConstraint(const std::string& constraint) {
        constraints.push_back(constraint);
    }
    
    std::string toString() const {
        if (constraints.empty()) {
            return name;
        }
        std::string result = name + ": ";
        for (size_t i = 0; i < constraints.size(); i++) {
            if (i > 0) result += " + ";
            result += constraints[i];
        }
        return result;
    }
};

// Generic type (e.g., fn<T>(T) -> T)
class GenericType : public Type {
private:
    std::vector<TypeParameter> type_params;
    std::shared_ptr<Type> inner_type;
    
public:
    GenericType(const std::vector<TypeParameter>& params, std::shared_ptr<Type> inner)
        : Type(TypeKind::GENERIC), type_params(params), inner_type(inner) {}
    
    const std::vector<TypeParameter>& getTypeParams() const { return type_params; }
    std::shared_ptr<Type> getInnerType() const { return inner_type; }
    
    // Instantiate generic type with concrete types
    std::shared_ptr<Type> instantiate(const std::vector<std::shared_ptr<Type>>& args) const {
        if (args.size() != type_params.size()) {
            throw std::runtime_error("Wrong number of type arguments");
        }
        
        // Create substitution map
        std::map<std::string, std::shared_ptr<Type>> subst_map;
        for (size_t i = 0; i < type_params.size(); i++) {
            subst_map[type_params[i].getName()] = args[i];
        }
        
        // Apply substitution to inner type
        return substituteTypeParams(inner_type, subst_map);
    }
    
    std::string toString() const override {
        std::string result = "<";
        for (size_t i = 0; i < type_params.size(); i++) {
            if (i > 0) result += ", ";
            result += type_params[i].toString();
        }
        result += ">" + inner_type->toString();
        return result;
    }
    
    bool equals(const Type& other) const override {
        if (!other.isGeneric()) return false;
        const auto& other_gen = static_cast<const GenericType&>(other);
        
        if (type_params.size() != other_gen.type_params.size()) return false;
        
        return inner_type->equals(*other_gen.inner_type);
    }
    
    std::shared_ptr<Type> clone() const override {
        return std::make_shared<GenericType>(type_params, inner_type->clone());
    }
    
private:
    // Helper to substitute type parameters
    std::shared_ptr<Type> substituteTypeParams(
        std::shared_ptr<Type> type,
        const std::map<std::string, std::shared_ptr<Type>>& subst_map) const {
        
        // If it's a type variable matching a parameter name, substitute
        if (type->isTypeVar()) {
            auto var = std::static_pointer_cast<TypeVariable>(type);
            auto it = subst_map.find(var->getName());
            if (it != subst_map.end()) {
                return it->second;
            }
        }
        
        // Recursively substitute in compound types
        if (type->isList()) {
            auto list_type = std::static_pointer_cast<ListType>(type);
            auto elem = substituteTypeParams(list_type->getElementType(), subst_map);
            return std::make_shared<ListType>(elem);
        }
        
        if (type->isFunction()) {
            auto func_type = std::static_pointer_cast<FunctionType>(type);
            std::vector<std::shared_ptr<Type>> params;
            for (const auto& p : func_type->getParamTypes()) {
                params.push_back(substituteTypeParams(p, subst_map));
            }
            auto ret = substituteTypeParams(func_type->getReturnType(), subst_map);
            return std::make_shared<FunctionType>(params, ret);
        }
        
        return type;
    }
};

// Helper to check if a type satisfies constraints
class ConstraintChecker {
public:
    static bool satisfiesConstraint(std::shared_ptr<Type> type, const std::string& constraint) {
        // Check if type satisfies a trait constraint
        
        if (constraint == "Numeric") {
            return type->isInt() || type->isFloat();
        }
        
        if (constraint == "Comparable") {
            return type->isInt() || type->isFloat() || type->isString();
        }
        
        if (constraint == "Eq") {
            return type->isInt() || type->isFloat() || type->isString() || type->isBool();
        }
        
        if (constraint == "Iterable") {
            return type->isList();
        }
        
        // Unknown constraint - assume satisfied for now
        return true;
    }
    
    static bool satisfiesConstraints(std::shared_ptr<Type> type, 
                                    const std::vector<std::string>& constraints) {
        for (const auto& constraint : constraints) {
            if (!satisfiesConstraint(type, constraint)) {
                return false;
            }
        }
        return true;
    }
};

} // namespace sapphire

#endif // SAPPHIRE_GENERIC_TYPE_H
