#include "../src/semantic/type_inference.h"
#include "../src/semantic/unification.h"
#include "../src/semantic/substitution.h"
#include "../src/types/type.h"
#include <cassert>
#include <iostream>

using namespace sapphire;

void test_substitution() {
    Substitution sub;
    
    auto var1 = std::make_shared<TypeVariable>();
    auto int_type = std::make_shared<IntType>();
    
    sub.add(var1->getId(), int_type);
    
    auto result = sub.apply(var1);
    assert(result->isInt());
    
    std::cout << "✓ test_substitution passed\n";
}

void test_unification_primitives() {
    auto int1 = std::make_shared<IntType>();
    auto int2 = std::make_shared<IntType>();
    
    Substitution sub = Unifier::unify(int1, int2);
    assert(sub.empty());
    
    std::cout << "✓ test_unification_primitives passed\n";
}

void test_unification_type_var() {
    auto var = std::make_shared<TypeVariable>();
    auto int_type = std::make_shared<IntType>();
    
    Substitution sub = Unifier::unify(var, int_type);
    assert(sub.has(var->getId()));
    
    auto result = sub.apply(var);
    assert(result->isInt());
    
    std::cout << "✓ test_unification_type_var passed\n";
}

void test_unification_lists() {
    auto var = std::make_shared<TypeVariable>();
    auto list1 = std::make_shared<ListType>(var);
    auto list2 = std::make_shared<ListType>(std::make_shared<IntType>());
    
    Substitution sub = Unifier::unify(list1, list2);
    
    auto result = sub.apply(list1);
    assert(result->isList());
    
    auto list_result = std::static_pointer_cast<ListType>(result);
    assert(list_result->getElementType()->isInt());
    
    std::cout << "✓ test_unification_lists passed\n";
}

void test_unification_functions() {
    auto var1 = std::make_shared<TypeVariable>();
    auto var2 = std::make_shared<TypeVariable>();
    
    auto func1 = std::make_shared<FunctionType>(
        std::vector<std::shared_ptr<Type>>{var1},
        var2
    );
    
    auto func2 = std::make_shared<FunctionType>(
        std::vector<std::shared_ptr<Type>>{std::make_shared<IntType>()},
        std::make_shared<StringType>()
    );
    
    Substitution sub = Unifier::unify(func1, func2);
    
    auto result = sub.apply(func1);
    assert(result->isFunction());
    
    auto func_result = std::static_pointer_cast<FunctionType>(result);
    assert(func_result->getParamTypes()[0]->isInt());
    assert(func_result->getReturnType()->isString());
    
    std::cout << "✓ test_unification_functions passed\n";
}

void test_occurs_check() {
    auto var = std::make_shared<TypeVariable>();
    auto list = std::make_shared<ListType>(var);
    
    try {
        Unifier::unify(var, list);
        assert(false && "Should have thrown UnificationError");
    } catch (const UnificationError& e) {
        // Expected
        std::cout << "✓ test_occurs_check passed\n";
    }
}

int main() {
    std::cout << "Running Type Inference Tests...\n\n";
    
    test_substitution();
    test_unification_primitives();
    test_unification_type_var();
    test_unification_lists();
    test_unification_functions();
    test_occurs_check();
    
    std::cout << "\n✅ All type inference tests passed!\n";
    return 0;
}
