#ifndef SAPPHIRE_TYPE_ENVIRONMENT_H
#define SAPPHIRE_TYPE_ENVIRONMENT_H

#include "../types/type.h"
#include <map>
#include <string>
#include <memory>
#include <stdexcept>

namespace sapphire {

class TypeEnvironment : public std::enable_shared_from_this<TypeEnvironment> {
private:
    std::map<std::string, std::shared_ptr<Type>> bindings;
    std::shared_ptr<TypeEnvironment> parent;
    
public:
    TypeEnvironment() : parent(nullptr) {}
    explicit TypeEnvironment(std::shared_ptr<TypeEnvironment> p) : parent(p) {}
    
    void define(const std::string& name, std::shared_ptr<Type> type) {
        bindings[name] = type;
    }
    
    std::shared_ptr<Type> get(const std::string& name) const {
        auto it = bindings.find(name);
        if (it != bindings.end()) {
            return it->second;
        }
        
        if (parent) {
            return parent->get(name);
        }
        
        throw std::runtime_error("Undefined variable '" + name + "'");
    }
    
    bool has(const std::string& name) const {
        if (bindings.find(name) != bindings.end()) {
            return true;
        }
        return parent && parent->has(name);
    }
    
    std::shared_ptr<TypeEnvironment> createChild() {
        return std::make_shared<TypeEnvironment>(shared_from_this());
    }
};

} // namespace sapphire

#endif // SAPPHIRE_TYPE_ENVIRONMENT_H
