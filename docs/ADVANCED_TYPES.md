# Advanced Type System

Sapphire features a cutting-edge type system with dependent types, higher-kinded types, and type-level computation.

## Dependent Types

Types that depend on values, enabling compile-time guarantees about runtime behavior.

### Fixed-Size Vectors

```sapphire
type Vec<T, n: usize> {
    data: [T; n]
}

fn create_vec3() -> Vec<f64, 3> {
    return Vec { data: [1.0, 2.0, 3.0] }
}
```

### Dimension-Safe Matrix Operations

```sapphire
fn matrix_multiply<T, m: usize, n: usize, p: usize>(
    a: Matrix<T, m, n>,
    b: Matrix<T, n, p>
) -> Matrix<T, m, p> {
    // Compiler ensures dimensions match
    // n must be the same in both matrices
}
```

### Length-Preserving Operations

```sapphire
fn concat<T, m: usize, n: usize>(
    a: Vec<T, m>,
    b: Vec<T, n>
) -> Vec<T, Add(m, n)> {
    // Type system proves result has length m + n
}
```

## Higher-Kinded Types

Types that take type constructors as parameters, enabling powerful abstractions.

### Functor

```sapphire
trait Functor<F<_>> {
    fn map<A, B>(self: F<A>, f: fn(A) -> B) -> F<B>
}
```

### Monad

```sapphire
trait Monad<M<_>>: Functor<M> {
    fn pure<A>(value: A) -> M<A>
    fn flat_map<A, B>(self: M<A>, f: fn(A) -> M<B>) -> M<B>
}

impl Monad<Option> for Option<T> {
    fn pure<A>(value: A) -> Option<A> {
        return Some(value)
    }
    
    fn flat_map<A, B>(self: Option<A>, f: fn(A) -> Option<B>) -> Option<B> {
        match self {
            Some(x) => f(x),
            None => None
        }
    }
}
```

### Usage

```sapphire
let result = Some(42)
    .flat_map(|x| Some(x * 2))
    .flat_map(|x| Some(x + 10))
// result = Some(94)
```

## Type-Level Computation

Compute types at compile time using type families.

### Type-Level Arithmetic

```sapphire
type family Add(a: Nat, b: Nat) -> Nat {
    Add(Zero, n) = n
    Add(Succ(m), n) = Succ(Add(m, n))
}

type family Multiply(a: Nat, b: Nat) -> Nat {
    Multiply(Zero, n) = Zero
    Multiply(Succ(m), n) = Add(n, Multiply(m, n))
}
```

### Type-Level Lists

```sapphire
type family Append(a: List, b: List) -> List {
    Append(Nil, ys) = ys
    Append(Cons(x, xs), ys) = Cons(x, Append(xs, ys))
}
```

## Phantom Types

Types that exist only at compile time for state tracking.

### State Machine

```sapphire
type Locked = State<"locked">
type Unlocked = State<"unlocked">

type Door<S> {
    state: S
}

fn lock(door: Door<Unlocked>) -> Door<Locked> {
    return Door { state: Locked }
}

fn unlock(door: Door<Locked>) -> Door<Unlocked> {
    return Door { state: Unlocked }
}

// This won't compile
// fn double_lock(door: Door<Locked>) -> Door<Locked> {
//     return lock(door)  // Type error!
// }
```

## Refinement Types

Types with predicates that constrain values.

### Basic Refinements

```sapphire
type NonZero = { x: i64 | x != 0 }
type Positive = { x: i64 | x > 0 }
type Negative = { x: i64 | x < 0 }
type Range<min: i64, max: i64> = { x: i64 | min <= x && x <= max }
```

### Safe Division

```sapphire
fn safe_divide(a: i64, b: NonZero) -> i64 {
    // Compiler guarantees b is never zero
    return a / b
}
```

### Bounded Arrays

```sapphire
fn get_element<n: usize>(arr: [i64; n], idx: Range<0, n-1>) -> i64 {
    // Compiler guarantees idx is in bounds
    return arr[idx]
}
```

## GADTs (Generalized Algebraic Data Types)

Types with different type parameters in different constructors.

### Type-Safe Expression Evaluator

```sapphire
type Expr<T> {
    IntLit(i64): Expr<i64>
    BoolLit(bool): Expr<bool>
    Add(Expr<i64>, Expr<i64>): Expr<i64>
    Eq(Expr<i64>, Expr<i64>): Expr<bool>
    If<T>(Expr<bool>, Expr<T>, Expr<T>): Expr<T>
}

fn eval<T>(expr: Expr<T>) -> T {
    match expr {
        IntLit(n) => n,
        BoolLit(b) => b,
        Add(a, b) => eval(a) + eval(b),
        Eq(a, b) => eval(a) == eval(b),
        If(cond, then_expr, else_expr) => {
            if eval(cond) {
                eval(then_expr)
            } else {
                eval(else_expr)
            }
        }
    }
}
```

## Linear Types

Types that must be used exactly once, preventing resource leaks.

### File Handles

```sapphire
type File: Linear {
    handle: FileHandle
}

fn open(path: String) -> File {
    return File { handle: os::open(path) }
}

fn close(file: File) {
    // Consumes file, can't be used again
    os::close(file.handle)
}

fn read(file: &File) -> String {
    // Borrows file, doesn't consume it
    return os::read(file.handle)
}

fn example() {
    let file = open("data.txt")
    let content = read(&file)
    close(file)
    // file can't be used here - already consumed
}
```

## Type Classes with Associated Types

```sapphire
trait Container {
    type Item
    type Iter: Iterator<Item = Self::Item>
    
    fn iter(self) -> Self::Iter
    fn len(self) -> usize
}

impl Container for Vec<T> {
    type Item = T
    type Iter = VecIter<T>
    
    fn iter(self) -> VecIter<T> {
        return VecIter::new(self)
    }
    
    fn len(self) -> usize {
        return self.length
    }
}
```

## Existential Types

Types that hide implementation details.

```sapphire
type exists T. Container<T> = {
    value: T,
    operations: ContainerOps<T>
}

fn create_container<T>(value: T) -> exists T. Container<T> {
    return Container {
        value: value,
        operations: default_ops()
    }
}
```

## Benefits

1. **Compile-Time Guarantees**: Catch errors before runtime
2. **Zero-Cost Abstractions**: No runtime overhead
3. **Better Documentation**: Types express intent
4. **Refactoring Safety**: Type system guides changes
5. **Performance**: Compiler can optimize better

## Examples

See `examples/advanced_types.spp` for complete examples.

## Further Reading

- [Type Inference](TYPE_INFERENCE.md)
- [Generics](GENERICS.md)
- [Language Features](LANGUAGE_FEATURES.md)
