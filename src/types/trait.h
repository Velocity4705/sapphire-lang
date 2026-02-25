#ifndef SAPPHIRE_TRAIT_H
#define SAPPHIRE_TRAIT_H

#include "type.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace sapphire {

// Method signature in a trait
struct TraitMethod {
    std::string name;
    std::vector<std::shared_ptr<Type>> param_types;
    std::shared_ptr<Type> return_type;
    
    TraitMethod(const std::string& n, 
                std::vector<std::shared_ptr<Type>> params,
                std::shared_ptr<Type> ret)
        : name(n), param_types(std::move(params)), return_type(ret) {}
    
    std::string toString() const {
        std::string result = "fn " + name + "(";
        for (size_t i = 0; i < param_types.size(); i++) {
            if (i > 0) result += ", ";
            result += param_types[i]->toString();
        }
        result += ") -> " + return_type->toString();
        return result;
    }
};

// Trait definition
class Trait {
private:
    std::string name;
    std::vector<TraitMethod> methods;
    std::vector<std::string> super_traits; // Trait inheritance
    
public:
    explicit Trait(const std::string& n) : name(n) {}
    
    std::string getName() const { return name; }
    const std::vector<TraitMethod>& getMethods() const { return methods; }
    const std::vector<std::string>& getSuperTraits() const { return super_traits; }
    
    void addMethod(const TraitMethod& method) {
        methods.push_back(method);
    }
    
    void addSuperTrait(const std::string& trait_name) {
        super_traits.push_back(trait_name);
    }
    
    bool hasMethod(const std::string& method_name) const {
        for (const auto& m : methods) {
            if (m.name == method_name) return true;
        }
        return false;
    }
    
    const TraitMethod* getMethod(const std::string& method_name) const {
        for (const auto& m : methods) {
            if (m.name == method_name) return &m;
        }
        return nullptr;
    }
    
    std::string toString() const {
        std::string result = "trait " + name;
        if (!super_traits.empty()) {
            result += ": ";
            for (size_t i = 0; i < super_traits.size(); i++) {
                if (i > 0) result += " + ";
                result += super_traits[i];
            }
        }
        result += " {\n";
        for (const auto& m : methods) {
            result += "  " + m.toString() + "\n";
        }
        result += "}";
        return result;
    }
};

// Trait implementation for a type
class TraitImpl {
private:
    std::string trait_name;
    std::shared_ptr<Type> for_type;
    std::map<std::string, std::shared_ptr<FunctionType>> method_impls;
    
public:
    TraitImpl(const std::string& trait, std::shared_ptr<Type> type)
        : trait_name(trait), for_type(type) {}
    
    std::string getTraitName() const { return trait_name; }
    std::shared_ptr<Type> getForType() const { return for_type; }
    
    void addMethodImpl(const std::string& method_name, 
                      std::shared_ptr<FunctionType> func_type) {
        method_impls[method_name] = func_type;
    }
    
    bool hasMethodImpl(const std::string& method_name) const {
        return method_impls.find(method_name) != method_impls.end();
    }
    
    std::shared_ptr<FunctionType> getMethodImpl(const std::string& method_name) const {
        auto it = method_impls.find(method_name);
        if (it != method_impls.end()) {
            return it->second;
        }
        return nullptr;
    }
};

// Trait registry
class TraitRegistry {
private:
    std::map<std::string, Trait> traits;
    std::vector<TraitImpl> implementations;
    
    // Built-in traits
    void registerBuiltinTraits() {
        // Numeric trait
        Trait numeric("Numeric");
        numeric.addMethod(TraitMethod("add", 
            {std::make_shared<TypeVariable>("Self"), std::make_shared<TypeVariable>("Self")},
            std::make_shared<TypeVariable>("Self")));
        numeric.addMethod(TraitMethod("sub",
            {std::make_shared<TypeVariable>("Self"), std::make_shared<TypeVariable>("Self")},
            std::make_shared<TypeVariable>("Self")));
        traits["Numeric"] = numeric;
        
        // Comparable trait
        Trait comparable("Comparable");
        comparable.addMethod(TraitMethod("compare",
            {std::make_shared<TypeVariable>("Self"), std::make_shared<TypeVariable>("Self")},
            std::make_shared<IntType>()));
        traits["Comparable"] = comparable;
        
        // Eq trait
        Trait eq("Eq");
        eq.addMethod(TraitMethod("equals",
            {std::make_shared<TypeVariable>("Self"), std::make_shared<TypeVariable>("Self")},
            std::make_shared<BoolType>()));
        traits["Eq"] = eq;
        
        // Iterable trait
        Trait iterable("Iterable");
        auto elem_type = std::make_shared<TypeVariable>("T");
        iterable.addMethod(TraitMethod("iter",
            {std::make_shared<TypeVariable>("Self")},
            std::make_shared<ListType>(elem_type)));
        traits["Iterable"] = iterable;
    }
    
public:
    TraitRegistry() {
        registerBuiltinTraits();
    }
    
    void registerTrait(const Trait& trait) {
        traits[trait.getName()] = trait;
    }
    
    bool hasTrait(const std::string& name) const {
        return traits.find(name) != traits.end();
    }
    
    const Trait* getTrait(const std::string& name) const {
        auto it = traits.find(name);
        if (it != traits.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    void registerImpl(const TraitImpl& impl) {
        implementations.push_back(impl);
    }
    
    bool hasImpl(const std::string& trait_name, std::shared_ptr<Type> type) const {
        for (const auto& impl : implementations) {
            if (impl.getTraitName() == trait_name && 
                impl.getForType()->equals(*type)) {
                return true;
            }
        }
        return false;
    }
    
    const TraitImpl* getImpl(const std::string& trait_name, 
                            std::shared_ptr<Type> type) const {
        for (const auto& impl : implementations) {
            if (impl.getTraitName() == trait_name && 
                impl.getForType()->equals(*type)) {
                return &impl;
            }
        }
        return nullptr;
    }
};

} // namespace sapphire

#endif // SAPPHIRE_TRAIT_H
