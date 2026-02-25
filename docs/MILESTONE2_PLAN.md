# Milestone 2: Type System

**Duration**: Months 4-6  
**Status**: 🚧 In Progress  
**Goal**: Complete type system with inference

## Objectives

### 1. Type Representation
- [ ] Type class hierarchy
- [ ] Primitive types (int, float, string, bool, none)
- [ ] Compound types (List, Dict, Tuple)
- [ ] Function types
- [ ] Generic types (List<T>, Dict<K,V>)
- [ ] Union types (int | string)
- [ ] Optional types (T?)

### 2. Type Inference Engine
- [ ] Hindley-Milner style inference
- [ ] Constraint generation
- [ ] Constraint solving (unification)
- [ ] Type variable substitution
- [ ] Let-polymorphism

### 3. Type Checking
- [ ] Expression type checking
- [ ] Statement type checking
- [ ] Function signature checking
- [ ] Return type validation
- [ ] Assignment compatibility

### 4. Advanced Features
- [ ] Generic functions
- [ ] Trait/interface system
- [ ] Pattern matching with exhaustiveness
- [ ] Null safety (Option<T>)
- [ ] Type aliases

### 5. Error Messages
- [ ] Clear type mismatch errors
- [ ] Suggestions for fixes
- [ ] Error location highlighting
- [ ] Multiple error reporting

## Implementation Plan

### Phase 1: Type Representation (Week 1)
```
src/types/
├── type.h              # Base Type class
├── type.cpp
├── primitive_type.h    # int, float, string, bool
├── compound_type.h     # List, Dict, Tuple
├── function_type.h     # Function signatures
└── type_variable.h     # For inference
```

### Phase 2: Type Inference (Week 2-3)
```
src/semantic/
├── type_inference.h    # Inference engine
├── type_inference.cpp
├── constraints.h       # Type constraints
├── unification.h       # Unification algorithm
└── substitution.h      # Type substitution
```

### Phase 3: Type Checking (Week 4)
```
src/semantic/
├── type_checker.h      # Type checking visitor
├── type_checker.cpp
└── type_environment.h  # Type environment
```

### Phase 4: Integration (Week 5-6)
- Integrate with parser
- Update interpreter
- Add error reporting
- Write tests

## Success Criteria

✅ Type inference works correctly
✅ Generics compile and run
✅ Pattern matching is exhaustive
✅ Clear type error messages
✅ All tests pass

## Example Code to Support

```sapphire
# Type inference
let x = 42              # inferred as int
let y = 3.14            # inferred as float
let name = "Alice"      # inferred as string

# Explicit types
let count: int = 10
let ratio: float = 0.5

# Generic functions
fn max<T: Comparable>(a: T, b: T) -> T:
    return a if a > b else b

# Lists with type inference
let numbers = [1, 2, 3]           # List<int>
let mixed = [1, "two", 3.0]       # List<int | string | float>

# Optional types
let maybe_value: int? = None
if maybe_value:
    print(maybe_value)  # Smart cast to int

# Function types
fn apply(f: fn(int) -> int, x: int) -> int:
    return f(x)

# Pattern matching with types
match value:
    x: int => print("Integer: " + str(x))
    s: string => print("String: " + s)
    _ => print("Other")
```

## Testing Strategy

1. **Unit Tests**
   - Type inference for primitives
   - Generic type instantiation
   - Constraint solving
   - Unification algorithm

2. **Integration Tests**
   - Full programs with type checking
   - Error message quality
   - Performance benchmarks

3. **Example Programs**
   - Generic data structures
   - Type-safe collections
   - Pattern matching examples

## Performance Goals

- Type checking: < 100ms per 1000 LOC
- Inference: < 50ms per function
- Memory: < 10MB for type information

## Documentation

- [ ] Type system guide
- [ ] Type inference explanation
- [ ] Generic programming tutorial
- [ ] Error message reference
- [ ] API documentation

## Next Steps

1. Create type representation classes
2. Implement basic type inference
3. Add type checking to interpreter
4. Write comprehensive tests
5. Improve error messages
