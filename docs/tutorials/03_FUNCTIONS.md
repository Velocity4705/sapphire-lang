# Tutorial 3: Functions

Learn how to organize your code with functions.

## What are Functions?

Functions are reusable blocks of code that perform specific tasks. They help you:
- Organize code
- Avoid repetition
- Make code easier to read and maintain

## Basic Function

```sapphire
fn greet() {
    println("Hello, World!")
}

fn main() {
    greet()  // Call the function
    greet()  // Call it again
}
```

Output:
```
Hello, World!
Hello, World!
```

## Functions with Parameters

Pass data to functions:

```sapphire
fn greet(name: str) {
    println("Hello, " + name + "!")
}

fn main() {
    greet("Alice")
    greet("Bob")
    greet("Charlie")
}
```

Output:
```
Hello, Alice!
Hello, Bob!
Hello, Charlie!
```

### Multiple Parameters

```sapphire
fn introduce(name: str, age: int) {
    println("My name is " + name + " and I am " + str(age) + " years old")
}

fn main() {
    introduce("Alice", 25)
    introduce("Bob", 30)
}
```

## Return Values

Functions can return values:

```sapphire
fn add(a: int, b: int) -> int {
    return a + b
}

fn main() {
    let result = add(5, 3)
    println("5 + 3 = " + str(result))
}
```

### Multiple Return Statements

```sapphire
fn abs(x: int) -> int {
    if x < 0 {
        return -x
    }
    return x
}

fn main() {
    println(abs(-5))  // 5
    println(abs(3))   // 3
}
```

### Implicit Return

The last expression is automatically returned:

```sapphire
fn square(x: int) -> int {
    x * x  // No return keyword needed
}

fn main() {
    println(square(5))  // 25
}
```

## Function Examples

### Area Calculator

```sapphire
fn rectangle_area(width: float, height: float) -> float {
    width * height
}

fn circle_area(radius: float) -> float {
    3.14159 * radius * radius
}

fn main() {
    let rect = rectangle_area(5.0, 3.0)
    println("Rectangle area: " + str(rect))
    
    let circ = circle_area(4.0)
    println("Circle area: " + str(circ))
}
```

### Temperature Converter

```sapphire
fn celsius_to_fahrenheit(celsius: float) -> float {
    (celsius * 9.0 / 5.0) + 32.0
}

fn fahrenheit_to_celsius(fahrenheit: float) -> float {
    (fahrenheit - 32.0) * 5.0 / 9.0
}

fn main() {
    let temp_c = 25.0
    let temp_f = celsius_to_fahrenheit(temp_c)
    println(str(temp_c) + "°C = " + str(temp_f) + "°F")
    
    let temp_f2 = 77.0
    let temp_c2 = fahrenheit_to_celsius(temp_f2)
    println(str(temp_f2) + "°F = " + str(temp_c2) + "°C")
}
```

### Is Even/Odd

```sapphire
fn is_even(n: int) -> bool {
    n % 2 == 0
}

fn is_odd(n: int) -> bool {
    n % 2 != 0
}

fn main() {
    for i in 1..6 {
        if is_even(i) {
            println(str(i) + " is even")
        } else {
            println(str(i) + " is odd")
        }
    }
}
```

## Variable Scope

Variables defined inside a function are local to that function:

```sapphire
fn test() {
    let x = 10  // Local to test()
    println(x)
}

fn main() {
    test()
    // println(x)  // Error! x doesn't exist here
}
```

### Global vs Local

```sapphire
let global_var = 100  // Global variable

fn show_vars() {
    let local_var = 50  // Local variable
    println("Global: " + str(global_var))
    println("Local: " + str(local_var))
}

fn main() {
    show_vars()
    println("Global: " + str(global_var))
    // println(local_var)  // Error! local_var doesn't exist here
}
```

## Default Parameters

```sapphire
fn greet(name: str, greeting: str = "Hello") {
    println(greeting + ", " + name + "!")
}

fn main() {
    greet("Alice")              // Uses default: "Hello, Alice!"
    greet("Bob", "Hi")          // Custom greeting: "Hi, Bob!"
    greet("Charlie", "Hey")     // Custom greeting: "Hey, Charlie!"
}
```

## Recursive Functions

Functions that call themselves:

```sapphire
fn factorial(n: int) -> int {
    if n <= 1 {
        return 1
    }
    return n * factorial(n - 1)
}

fn main() {
    println("5! = " + str(factorial(5)))  // 120
    println("10! = " + str(factorial(10)))  // 3628800
}
```

### Fibonacci

```sapphire
fn fibonacci(n: int) -> int {
    if n <= 1 {
        return n
    }
    return fibonacci(n - 1) + fibonacci(n - 2)
}

fn main() {
    for i in 0..10 {
        println("fib(" + str(i) + ") = " + str(fibonacci(i)))
    }
}
```

## Practice Exercises

### Exercise 1: Max Function
Write a function that returns the maximum of two numbers:

```sapphire
fn max(a: int, b: int) -> int {
    if a > b {
        return a
    }
    return b
}

fn main() {
    println(max(10, 5))   // 10
    println(max(3, 8))    // 8
}
```

### Exercise 2: Power Function
Calculate x raised to the power of y:

```sapphire
fn power(x: int, y: int) -> int {
    let mut result = 1
    for i in 0..y {
        result = result * x
    }
    return result
}

fn main() {
    println("2^3 = " + str(power(2, 3)))   // 8
    println("5^2 = " + str(power(5, 2)))   // 25
}
```

### Exercise 3: Prime Checker
Check if a number is prime:

```sapphire
fn is_prime(n: int) -> bool {
    if n < 2 {
        return false
    }
    
    for i in 2..n {
        if n % i == 0 {
            return false
        }
    }
    
    return true
}

fn main() {
    for i in 1..20 {
        if is_prime(i) {
            println(str(i) + " is prime")
        }
    }
}
```

### Exercise 4: String Reverser
Reverse a string:

```sapphire
fn reverse(s: str) -> str {
    let mut result = ""
    for i in len(s)-1..0 step -1 {
        result = result + s[i]
    }
    return result
}

fn main() {
    println(reverse("hello"))     // olleh
    println(reverse("Sapphire"))  // erihppaS
}
```

### Exercise 5: Grade Calculator
Convert score to letter grade:

```sapphire
fn get_grade(score: int) -> str {
    if score >= 90 {
        return "A"
    } else if score >= 80 {
        return "B"
    } else if score >= 70 {
        return "C"
    } else if score >= 60 {
        return "D"
    } else {
        return "F"
    }
}

fn main() {
    let scores = [95, 87, 72, 65, 55]
    
    for score in scores {
        let grade = get_grade(score)
        println("Score " + str(score) + " = Grade " + grade)
    }
}
```

## Common Mistakes

### 1. Forgetting Return Type
```sapphire
// Wrong:
fn add(a: int, b: int) {  // Missing return type
    return a + b
}

// Correct:
fn add(a: int, b: int) -> int {
    return a + b
}
```

### 2. Not Returning a Value
```sapphire
// Wrong:
fn add(a: int, b: int) -> int {
    let sum = a + b
    // Forgot to return!
}

// Correct:
fn add(a: int, b: int) -> int {
    let sum = a + b
    return sum
}
```

### 3. Wrong Parameter Types
```sapphire
// Wrong:
fn greet(name: int) {  // Should be str, not int
    println("Hello, " + name)
}

// Correct:
fn greet(name: str) {
    println("Hello, " + name)
}
```

## Summary

In this tutorial, you learned:
- ✅ How to define functions
- ✅ Parameters and return values
- ✅ Variable scope
- ✅ Default parameters
- ✅ Recursive functions

## Next Steps

Continue to [Tutorial 4: Arrays and Collections](04_ARRAYS.md) to learn about:
- Arrays
- Lists
- Dictionaries
- Working with collections

Happy coding! 🚀
