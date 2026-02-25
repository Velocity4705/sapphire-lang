#ifndef SAPPHIRE_UNIFICATION_H
#define SAPPHIRE_UNIFICATION_H

#include "../types/type.h"
#include "substitution.h"
#include <memory>
#include <stdexcept>
#include <set>

namespace sapphire {

class UnificationError : public std::runtime_error {
public:
    explicit UnificationError(const std::string& msg) : std::runtime_error(msg) {}
};

class Unifier {
private:
    // Check if a type variable occurs in a type (occurs check)
    static bool occurs(int var_id, std::shared_ptr<Type> type, const Substitution& sub) {
        type = sub.apply(type);
        
        if (type->isTypeVar()) {
            auto var = std::static_pointer_cast<TypeVariable>(type);
            return var->getId() == var_id;
        }
        
        if (type->isList()) {
            auto list_type = std::static_pointer_cast<ListType>(type);
            return occurs(var_id, list_type->getElementType(), sub);
        }
        
        if (type->isFunction()) {
            auto func_type = std::static_pointer_cast<FunctionType>(type);
            for (const auto& param : func_type->getParamTypes()) {
                if (occurs(var_id, param, sub)) return true;
            }
            return occurs(var_id, func_type->getReturnType(), sub);
        }
        
        if (type->isOptional()) {
            auto opt_type = std::static_pointer_cast<OptionalType>(type);
            return occurs(var_id, opt_type->getInnerType(), sub);
        }
        
        return false;
    }
    
public:
    // Unify two types
    static Substitution unify(std::shared_ptr<Type> t1, std::shared_ptr<Type> t2, 
                             const Substitution& sub = Substitution()) {
        // Apply current substitution
        t1 = sub.apply(t1);
        t2 = sub.apply(t2);
        
        // Same type
        if (t1->equals(*t2)) {
            return sub;
        }
        
        // Type variable on left
        if (t1->isTypeVar()) {
            auto var = std::static_pointer_cast<TypeVariable>(t1);
            if (occurs(var->getId(), t2, sub)) {
                throw UnificationError("Occurs check failed: " + t1->toString() + 
                                      " occurs in " + t2->toString());
            }
            Substitution new_sub = sub;
            new_sub.add(var->getId(), t2);
            return new_sub;
        }
        
        // Type variable on right
        if (t2->isTypeVar()) {
            return unify(t2, t1, sub);
        }
        
        // Both are lists
        if (t1->isList() && t2->isList()) {
            auto list1 = std::static_pointer_cast<ListType>(t1);
            auto list2 = std::static_pointer_cast<ListType>(t2);
            return unify(list1->getElementType(), list2->getElementType(), sub);
        }
        
        // Both are functions
        if (t1->isFunction() && t2->isFunction()) {
            auto func1 = std::static_pointer_cast<FunctionType>(t1);
            auto func2 = std::static_pointer_cast<FunctionType>(t2);
            
            if (func1->getParamTypes().size() != func2->getParamTypes().size()) {
                throw UnificationError("Function arity mismatch: " + 
                                      t1->toString() + " vs " + t2->toString());
            }
            
            Substitution result = sub;
            
            // Unify parameter types
            for (size_t i = 0; i < func1->getParamTypes().size(); i++) {
                result = unify(func1->getParamTypes()[i], 
                             func2->getParamTypes()[i], result);
            }
            
            // Unify return types
            result = unify(func1->getReturnType(), func2->getReturnType(), result);
            
            return result;
        }
        
        // Both are optional
        if (t1->isOptional() && t2->isOptional()) {
            auto opt1 = std::static_pointer_cast<OptionalType>(t1);
            auto opt2 = std::static_pointer_cast<OptionalType>(t2);
            return unify(opt1->getInnerType(), opt2->getInnerType(), sub);
        }
        
        // Numeric compatibility: int can unify with float
        if ((t1->isInt() && t2->isFloat()) || (t1->isFloat() && t2->isInt())) {
            // Promote to float
            return sub;
        }
        
        // Cannot unify
        throw UnificationError("Cannot unify " + t1->toString() + 
                              " with " + t2->toString());
    }
    
    // Unify a list of type pairs
    static Substitution unifyAll(const std::vector<std::pair<std::shared_ptr<Type>, 
                                                             std::shared_ptr<Type>>>& pairs) {
        Substitution sub;
        for (const auto& [t1, t2] : pairs) {
            sub = unify(t1, t2, sub);
        }
        return sub;
    }
};

} // namespace sapphire

#endif // SAPPHIRE_UNIFICATION_H
