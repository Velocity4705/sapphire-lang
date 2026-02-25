# Sapphire Type Inference

## Overview

Sapphire uses a **Hindley-Milner** style type inference system that automatically deduces types without requiring explicit type annotations.

## How It Works

### 1. Type Variables

When the compiler encounters an expression without a known type, it creates a **type variable** (like `T0`, `T1`, etc.):

```sapphire
let x = 42        # x: T0
let y = x + 10    # y: T1
```

### 2. Constraint Generation

The compiler generates **constraints** based on how values are used:

```sapphire
let x = 42        # Constraint: T0 = int
let y = x + 10    # Constraints: T0 = T1, T1 = int
```

### 3. Unification

The **unification algorithm** solves these constraints to find the most general types:

```sapphire
# After unification:
# T0 = int
# T1 = int
# Therefore: x: int, y: int
```

## Examples

### Basic Inference

```sapphire
# Literals
let x = 42              # x: int
let y = 3.14            # y: float
let name = "Alice"      # name: string
let flag = true         # flag: bool

# Arithmetic
let sum = x + 10        # sum: int
let ratio = y / 2.0     # ratio: float
```

### List Inference

```sapphire
# Homogeneous lists
let numbers = [1, 2, 3]           # numbers: List<int>
let names = ["Alice", "Bob"]      # names: List<string>

# Empty list (generic)
let empty = []                    # empty: List<T>
```

### Function Inference

```sapphire
# Function without type annotations
fn double(x):
    return x * 2

# Inferred type: fn(int) -> int
# or more generally: fn(T: Numeric) -> T

let result = double(21)  # result: int
```

### Generic Functions

```sapphire
# Identity function
fn identity(x):
    return x

# Inferred type: fn<T>(T) -> T

let a = identity(42)      # a: int
let b = identity("hi")    # b: string
```

### Complex Inference

```sapphire
# Map function
fn map(f, list):
    let result = []
    for item in list:
        result.append(f(item))
    return result

# Inferred type: fn<T, U>(fn(T) -> U, List<T>) -> List<U>

fn square(x):
    return x * x

let squared = map(square, [1, 2, 3])  # squared: List<int>
```

## Type Constraints

### Equality Constraints

```sapphire
let x = 42
let y = x

# Constraint: type(x) = type(y)
# Solution: x: int, y: int
```

### Function Application

```sapphire
fn add(a, b):
    return a + b

let result = add(10, 20)

# Constraints:
# - type(add) = fn(T1, T2) -> T3
# - type(a) = T1, type(b) = T2
# - T1 = int, T2 = int
# - T3 = int (from a + b)
# Solution: add: fn(int, int) -> int
```

### List Operations

```sapphire
let numbers = [1, 2, 3]
let first = numbers[0]

# Constraints:
# - type(numbers) = List<T>
# - type(first) = T
# - T = int (from list elements)
# Solution: numbers: List<int>, first: int
```

## Unification Algorithm

The unification algorithm finds the most general unifier for two types:

```
unify(int, int) = {}                    # Same type
unify(T, int) = {T -> int}              # Bind type variable
unify(List<T>, List<int>) = {T -> int}  # Recursive unification
unify(int, string) = ERROR              # Type mismatch
```

### Occurs Check

Prevents infinite types:

```sapphire
# This would create: T = List<T> (infinite!)
# The occurs check prevents this
```

## Type Inference Rules

### Literals

```
─────────────────
Γ ⊢ 42 : int

─────────────────
Γ ⊢ 3.14 : float

─────────────────
Γ ⊢ "hi" : string
```

### Variables

```
x : T ∈ Γ
─────────────────
Γ ⊢ x : T
```

### Function Application

```
Γ ⊢ f : T1 -> T2    Γ ⊢ x : T1
──────────────────────────────
Γ ⊢ f(x) : T2
```

### Let Binding

```
Γ ⊢ e1 : T1    Γ, x : T1 ⊢ e2 : T2
────────────────────────────────────
Γ ⊢ let x = e1 in e2 : T2
```

## Benefits

1. **Less Boilerplate**: No need to write types everywhere
2. **Type Safety**: Still catches type errors at compile time
3. **Flexibility**: Works with generic code naturally
4. **Clarity**: Inferred types are often obvious from context

## Limitations

1. **Error Messages**: Can be cryptic when inference fails
2. **Performance**: Type inference can be slow for large programs
3. **Ambiguity**: Sometimes explicit types are needed

## Best Practices

### When to Use Explicit Types

```sapphire
# Good: Explicit types for public APIs
fn process_data(data: List<int>) -> Result<string, Error>:
    # ...

# Good: Explicit types for clarity
let config: Config = load_config()

# Good: When inference is ambiguous
let empty: List<int> = []
```

### When to Rely on Inference

```sapphire
# Good: Local variables with obvious types
let count = 42
let name = "Alice"

# Good: Intermediate calculations
let sum = numbers.reduce(lambda a, b: a + b, 0)

# Good: Generic helper functions
fn identity(x):
    return x
```

## Implementation Details

### Constraint Solver

The constraint solver uses:
- **Unification**: Robinson's algorithm
- **Substitution**: Type variable replacement
- **Occurs Check**: Prevents infinite types

### Type Variables

- Fresh type variables: `T0`, `T1`, `T2`, ...
- Scoped to inference context
- Replaced with concrete types after solving

### Polymorphism

Sapphire supports **let-polymorphism**:

```sapphire
fn identity(x):
    return x

# Can be used with different types
let a = identity(42)      # int
let b = identity("hi")    # string
```

## Future Enhancements

- [ ] Rank-N types
- [ ] Type classes/traits
- [ ] Dependent types
- [ ] Refinement types
- [ ] Gradual typing
