#ifndef SAPPHIRE_SUBSTITUTION_H
#define SAPPHIRE_SUBSTITUTION_H

#include "../types/type.h"
#include <map>
#include <memory>

namespace sapphire {

// Type substitution: maps type variables to types
class Substitution {
private:
    std::map<int, std::shared_ptr<Type>> mappings;
    
public:
    Substitution() = default;
    
    // Add a substitution
    void add(int var_id, std::shared_ptr<Type> type) {
        mappings[var_id] = type;
    }
    
    // Get substitution for a type variable
    std::shared_ptr<Type> get(int var_id) const {
        auto it = mappings.find(var_id);
        if (it != mappings.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Check if a type variable has a substitution
    bool has(int var_id) const {
        return mappings.find(var_id) != mappings.end();
    }
    
    // Apply substitution to a type
    std::shared_ptr<Type> apply(std::shared_ptr<Type> type) const {
        if (type->isTypeVar()) {
            auto var = std::static_pointer_cast<TypeVariable>(type);
            auto sub = get(var->getId());
            if (sub) {
                // Recursively apply substitution
                return apply(sub);
            }
            return type;
        }
        
        if (type->isList()) {
            auto list_type = std::static_pointer_cast<ListType>(type);
            auto elem_type = apply(list_type->getElementType());
            return std::make_shared<ListType>(elem_type);
        }
        
        if (type->isFunction()) {
            auto func_type = std::static_pointer_cast<FunctionType>(type);
            std::vector<std::shared_ptr<Type>> param_types;
            for (const auto& param : func_type->getParamTypes()) {
                param_types.push_back(apply(param));
            }
            auto return_type = apply(func_type->getReturnType());
            return std::make_shared<FunctionType>(param_types, return_type);
        }
        
        if (type->isOptional()) {
            auto opt_type = std::static_pointer_cast<OptionalType>(type);
            auto inner = apply(opt_type->getInnerType());
            return std::make_shared<OptionalType>(inner);
        }
        
        return type;
    }
    
    // Compose two substitutions
    Substitution compose(const Substitution& other) const {
        Substitution result;
        
        // Apply other to all mappings in this
        for (const auto& [var_id, type] : mappings) {
            result.add(var_id, other.apply(type));
        }
        
        // Add mappings from other that aren't in this
        for (const auto& [var_id, type] : other.mappings) {
            if (!has(var_id)) {
                result.add(var_id, type);
            }
        }
        
        return result;
    }
    
    // Check if empty
    bool empty() const {
        return mappings.empty();
    }
    
    // Get size
    size_t size() const {
        return mappings.size();
    }
    
    // Clear all substitutions
    void clear() {
        mappings.clear();
    }
    
    // String representation
    std::string toString() const {
        std::string result = "Substitution:\n";
        for (const auto& [var_id, type] : mappings) {
            result += "  T" + std::to_string(var_id) + " -> " + type->toString() + "\n";
        }
        return result;
    }
};

} // namespace sapphire

#endif // SAPPHIRE_SUBSTITUTION_H
