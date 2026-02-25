# Sapphire Syntax Specification

## Design Principles

1. **Readability First**: Clear, unambiguous syntax
2. **Minimal Boilerplate**: Get things done with less code
3. **Type Safety**: Catch errors at compile time
4. **Performance**: Zero-cost abstractions where possible

## Basic Syntax Examples

### Variables and Types

```sapphire
# Type inference
let x = 42
let name = "Sapphire"

# Explicit typing
let count: int = 100
let ratio: float = 3.14

# Constants
const MAX_SIZE = 1000
```

### Functions

```sapphire
fn add(a: int, b: int) -> int:
    return a + b

fn greet(name: str):
    print("Hello, " + name)
```

### Control Flow

```sapphire
# If statements
if x > 10:
    print("Large")
elif x > 5:
    print("Medium")
else:
    print("Small")

# Loops
for i in range(10):
    print(i)

while condition:
    do_something()
```

### Data Structures

```sapphire
# Lists
numbers = [1, 2, 3, 4, 5]

# Dictionaries
person = {"name": "Alice", "age": 30}

# Tuples
point = (10, 20)
```

### Error Handling

```sapphire
fn divide(a: int, b: int) -> Result<int, str>:
    if b == 0:
        return Err("Division by zero")
    return Ok(a / b)

# Usage
match divide(10, 2):
    Ok(result) => print(result)
    Err(msg) => print("Error: " + msg)
```

## Memory Management

- Automatic garbage collection (mark-and-sweep or generational)
- No manual memory management required
- Optional reference counting for deterministic cleanup
