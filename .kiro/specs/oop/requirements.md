# Object-Oriented Programming (OOP) Feature Specification

## Feature Name
Object-Oriented Programming (Classes, Objects, Inheritance)

## Overview
Implement comprehensive OOP support for Sapphire v1.0.0, including classes, objects, methods, properties, inheritance, and constructors. This will bring the language to 97% completion.

## User Stories

### US-1: Class Definition
**As a** developer  
**I want to** define classes with methods and properties  
**So that** I can organize my code using object-oriented principles

**Acceptance Criteria:**
1. Can define classes with `class` keyword
2. Classes can have methods
3. Classes can have properties
4. Can create objects from classes
5. Can access object properties and methods

### US-2: Constructors
**As a** developer  
**I want to** initialize objects with constructors  
**So that** I can set up object state when creating instances

**Acceptance Criteria:**
1. Can define `__init__` constructor method
2. Constructor can accept parameters
3. Can initialize instance properties in constructor
4. Constructor is called automatically when creating objects

### US-3: Methods and Properties
**As a** developer  
**I want to** define methods and access properties  
**So that** I can encapsulate behavior and data

**Acceptance Criteria:**
1. Can define instance methods
2. Methods can access `self` to reference the instance
3. Can access properties using dot notation
4. Can call methods using dot notation

### US-4: Inheritance
**As a** developer  
**I want to** create class hierarchies with inheritance  
**So that** I can reuse and extend existing functionality

**Acceptance Criteria:**
1. Can inherit from parent classes
2. Child classes inherit parent methods and properties
3. Can override parent methods
4. Can call parent methods using `super()`

### US-5: Encapsulation
**As a** developer  
**I want to** control access to class members  
**So that** I can enforce encapsulation principles

**Acceptance Criteria:**
1. Can define public methods and properties
2. Can define private members (convention-based)
3. Can access public members from outside
4. Private members are accessible only within the class

## Syntax Design

### Class Definition
```sapphire
class Point:
    fn __init__(self, x, y):
        self.x = x
        self.y = y
    
    fn distance(self):
        return (self.x ** 2 + self.y ** 2) ** 0.5
```

### Object Creation and Usage
```sapphire
p = Point(3, 4)
print(p.x)  # 3
print(p.y)  # 4
print(p.distance())  # 5.0
```

### Inheritance
```sapphire
class Animal:
    fn __init__(self, name):
        self.name = name
    
    fn speak(self):
        print("...")

class Dog(Animal):
    fn __init__(self, name, breed):
        super().__init__(name)
        self.breed = breed
    
    fn speak(self):
        print("Woof!")
```

### Method Overriding
```sapphire
class Shape:
    fn area(self):
        return 0

class Rectangle(Shape):
    fn __init__(self, width, height):
        self.width = width
        self.height = height
    
    fn area(self):
        return self.width * self.height
```

## Technical Requirements

### Parser Changes
1. Add `ClassDecl` to AST (already exists)
2. Parse class definitions with inheritance
3. Parse method definitions within classes
4. Parse `self` parameter in methods
5. Parse `super()` calls

### Interpreter Changes
1. Implement class objects
2. Implement instance creation
3. Implement method binding
4. Implement property access
5. Implement inheritance chain
6. Implement `super()` resolution

### Type System Changes
1. Add class types to type system
2. Type check method calls
3. Type check property access
4. Validate inheritance relationships

## Implementation Plan

### Phase 1: Basic Class Structure (2 hours)
1. Add class parsing support
2. Create class objects in interpreter
3. Implement basic object creation

### Phase 2: Methods and Properties (3 hours)
1. Implement method definitions
2. Implement property access
3. Implement `self` parameter handling
4. Implement method calls on objects

### Phase 3: Constructors (2 hours)
1. Implement `__init__` method
2. Automatic constructor invocation
3. Property initialization

### Phase 4: Inheritance (3 hours)
1. Implement inheritance parsing
2. Implement method resolution order
3. Implement `super()` calls
4. Implement method overriding

### Phase 5: Testing (2 hours)
1. Create comprehensive test suite
2. Test all OOP features
3. Test edge cases and error handling

## Test Cases

### Test 1: Basic Class
```sapphire
class Person:
    fn __init__(self, name, age):
        self.name = name
        self.age = age
    
    fn greet(self):
        return "Hello, " + self.name

p = Person("Alice", 30)
print(p.greet())  # "Hello, Alice"
```

### Test 2: Property Access
```sapphire
class Point:
    fn __init__(self, x, y):
        self.x = x
        self.y = y

p = Point(3, 4)
print(p.x)  # 3
print(p.y)  # 4
```

### Test 3: Inheritance
```sapphire
class Animal:
    fn speak(self):
        return "..."

class Dog(Animal):
    fn speak(self):
        return "Woof!"

d = Dog()
print(d.speak())  # "Woof!"
```

### Test 4: Super Calls
```sapphire
class Base:
    fn __init__(self, value):
        self.value = value

class Derived(Base):
    fn __init__(self, value, extra):
        super().__init__(value)
        self.extra = extra

d = Derived(10, 20)
print(d.value)  # 10
print(d.extra)  # 20
```

### Test 5: Method Overriding
```sapphire
class Shape:
    fn area(self):
        return 0

class Square(Shape):
    fn __init__(self, side):
        self.side = side
    
    fn area(self):
        return self.side * self.side

s = Square(5)
print(s.area())  # 25
```

### Test 6: Multiple Methods
```sapphire
class Calculator:
    fn add(self, a, b):
        return a + b
    
    fn multiply(self, a, b):
        return a * b

calc = Calculator()
print(calc.add(5, 3))  # 8
print(calc.multiply(5, 3))  # 15
```

## Success Criteria

1. All test cases pass
2. No regression in existing functionality
3. Performance impact is minimal
4. Error messages are clear and helpful
5. Documentation is complete

## Estimated Effort
- Total: 12-14 hours
- Parser: 3 hours
- Interpreter: 6 hours
- Type System: 2 hours
- Testing: 2 hours
- Documentation: 1 hour

## Dependencies
- Existing AST already has ClassDecl
- Existing interpreter has some OOP infrastructure
- Need to complete missing implementations

## Risks
1. Method resolution order can be complex
2. Inheritance chain implementation needs careful design
3. `super()` implementation requires parent class tracking
4. Performance of method lookups

## Alternatives Considered
1. Prototype-based inheritance (JavaScript-style) - Not chosen
2. Structs with methods (Go-style) - Not chosen
3. Traits/mixins (Rust-style) - Deferred to v1.1

## Notes
- Keep implementation simple for v1.0
- Focus on Python-like syntax and semantics
- Advanced features (multiple inheritance, abstract classes) can wait for v1.1
- Use simple single inheritance for v1.0