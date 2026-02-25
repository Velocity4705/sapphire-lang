#ifndef SAPPHIRE_CONSTRAINTS_H
#define SAPPHIRE_CONSTRAINTS_H

#include "../types/type.h"
#include <memory>
#include <vector>
#include <string>

namespace sapphire {

// Type constraint: T1 = T2
class Constraint {
private:
    std::shared_ptr<Type> left;
    std::shared_ptr<Type> right;
    std::string context; // For error messages
    
public:
    Constraint(std::shared_ptr<Type> l, std::shared_ptr<Type> r, const std::string& ctx = "")
        : left(l), right(r), context(ctx) {}
    
    std::shared_ptr<Type> getLeft() const { return left; }
    std::shared_ptr<Type> getRight() const { return right; }
    std::string getContext() const { return context; }
    
    std::string toString() const {
        return left->toString() + " = " + right->toString();
    }
};

// Collection of constraints
class ConstraintSet {
private:
    std::vector<Constraint> constraints;
    
public:
    void add(const Constraint& constraint) {
        constraints.push_back(constraint);
    }
    
    void add(std::shared_ptr<Type> left, std::shared_ptr<Type> right, 
             const std::string& context = "") {
        constraints.emplace_back(left, right, context);
    }
    
    const std::vector<Constraint>& getConstraints() const {
        return constraints;
    }
    
    size_t size() const {
        return constraints.size();
    }
    
    bool empty() const {
        return constraints.empty();
    }
    
    void clear() {
        constraints.clear();
    }
    
    std::string toString() const {
        std::string result = "Constraints:\n";
        for (const auto& c : constraints) {
            result += "  " + c.toString() + "\n";
        }
        return result;
    }
};

} // namespace sapphire

#endif // SAPPHIRE_CONSTRAINTS_H
