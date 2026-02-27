# Error Handling Feature Specification

## Feature Name
Error Handling (try-catch-finally)

## Overview
Implement comprehensive error handling for Sapphire v1.0.0, including try-catch-finally blocks, exception types, and error propagation. This will bring the language to 92% completion.

## User Stories

### US-1: Basic Try-Catch
**As a** developer  
**I want to** catch and handle runtime errors  
**So that** my program doesn't crash unexpectedly

**Acceptance Criteria:**
1. Can write try-catch blocks with Python-like syntax
2. Exceptions are caught and handled gracefully
3. Program continues execution after handling error
4. Can access error message in catch block

### US-2: Multiple Catch Blocks
**As a** developer  
**I want to** handle different types of errors differently  
**So that** I can provide specific error handling for each case

**Acceptance Criteria:**
1. Can write multiple catch blocks for different exception types
2. First matching catch block is executed
3. Can catch specific exception types (FileNotFoundError, DivisionByZeroError, etc.)
4. Can use generic catch block as fallback

### US-3: Finally Block
**As a** developer  
**I want to** execute cleanup code regardless of whether an error occurred  
**So that** resources are properly released

**Acceptance Criteria:**
1. Finally block always executes
2. Executes after try block if no error
3. Executes after catch block if error occurred
4. Can be used without catch block (try-finally)

### US-4: Throw Statement
**As a** developer  
**I want to** raise custom exceptions  
**So that** I can signal errors in my code

**Acceptance Criteria:**
1. Can throw exceptions with custom messages
2. Can throw built-in exception types
3. Thrown exceptions can be caught by try-catch
4. Can re-throw exceptions in catch blocks

### US-5: Built-in Exception Types
**As a** developer  
**I want to** use standard exception types  
**So that** I can categorize errors appropriately

**Acceptance Criteria:**
1. RuntimeError - generic runtime errors
2. DivisionByZeroError - division by zero
3. FileNotFoundError - file operations
4. IndexError - array out of bounds
5. TypeError - type mismatches
6. ValueError - invalid values

## Syntax Design

### Basic Try-Catch
```sapphire
try:
    risky_operation()
catch e:
    print("Error: " + e)
```

### Specific Exception Types
```sapphire
try:
    x = 10 / 0
catch DivisionByZeroError as e:
    print("Cannot divide by zero")
catch RuntimeError as e:
    print("Runtime error: " + e)
```

### Try-Finally
```sapphire
try:
    open_file()
finally:
    close_file()
```

### Try-Catch-Finally
```sapphire
try:
    process_file()
catch FileNotFoundError as e:
    print("File not found")
finally:
    cleanup()
```

### Throw Statement
```sapphire
fn validate_age(age):
    if age < 0:
        throw ValueError("Age cannot be negative")
    return age
```

## Technical Requirements

### Parser Changes
1. Add `TryStmt` to AST
2. Add `ThrowStmt` to AST
3. Parse `try`, `catch`, `finally` keywords
4. Parse exception type specifications
5. Parse `as` keyword for exception binding

### Interpreter Changes
1. Implement exception propagation mechanism
2. Create exception type hierarchy
3. Implement try-catch-finally execution
4. Implement throw statement
5. Handle exception matching by type

### Type System Changes
1. Add exception types to type system
2. Type check exception handlers
3. Validate exception types in throw statements

## Implementation Plan

### Phase 1: Basic Infrastructure (2 hours)
1. Add exception classes to interpreter
2. Implement basic exception propagation
3. Add try-catch AST nodes

### Phase 2: Parser Support (2 hours)
1. Add try-catch-finally parsing
2. Add throw statement parsing
3. Handle exception type specifications

### Phase 3: Interpreter Execution (2 hours)
1. Implement try-catch execution
2. Implement finally block execution
3. Implement throw statement
4. Add built-in exception types

### Phase 4: Testing (1 hour)
1. Create comprehensive test suite
2. Test all exception types
3. Test nested try-catch
4. Test finally block execution

## Test Cases

### Test 1: Basic Try-Catch
```sapphire
try:
    x = 10 / 0
catch e:
    print("Caught error")
# Expected: "Caught error"
```

### Test 2: Specific Exception Type
```sapphire
try:
    arr = [1, 2, 3]
    print(arr[10])
catch IndexError as e:
    print("Index out of bounds")
# Expected: "Index out of bounds"
```

### Test 3: Finally Block
```sapphire
result = 0
try:
    result = 10
finally:
    print("Finally executed")
print(result)
# Expected: "Finally executed" then "10"
```

### Test 4: Throw Statement
```sapphire
fn check_positive(n):
    if n < 0:
        throw ValueError("Number must be positive")
    return n

try:
    check_positive(-5)
catch ValueError as e:
    print("Validation failed")
# Expected: "Validation failed"
```

### Test 5: Multiple Catch Blocks
```sapphire
try:
    risky_operation()
catch FileNotFoundError as e:
    print("File error")
catch RuntimeError as e:
    print("Runtime error")
catch e:
    print("Unknown error")
```

### Test 6: Nested Try-Catch
```sapphire
try:
    try:
        x = 10 / 0
    catch e:
        print("Inner catch")
        throw e
catch e:
    print("Outer catch")
# Expected: "Inner catch" then "Outer catch"
```

## Success Criteria

1. All test cases pass
2. No regression in existing functionality
3. Error messages are clear and helpful
4. Performance impact is minimal
5. Documentation is complete

## Estimated Effort
- Total: 6-8 hours
- Parser: 2 hours
- Interpreter: 3 hours
- Testing: 1-2 hours
- Documentation: 1 hour

## Dependencies
- None (all prerequisites completed)

## Risks
1. Exception propagation may be complex
2. Finally block execution order needs careful handling
3. Type checking for exceptions may be tricky

## Alternatives Considered
1. Result<T, E> type (Rust-style) - Deferred to v1.1
2. Option<T> type - Deferred to v1.1
3. Panic/recover mechanism - Not suitable for Python-like language

## Notes
- Keep implementation simple for v1.0
- Focus on Python-like syntax and semantics
- Advanced features (custom exception classes) can wait for v1.1
