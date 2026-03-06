# Tutorial 2: Control Flow

Learn how to control the flow of your program with conditions and loops.

## If Statements

Make decisions in your code:

```sapphire
fn main() {
    let age = 18
    
    if age >= 18 {
        println("You are an adult")
    }
}
```

### If-Else

```sapphire
fn main() {
    let temperature = 25
    
    if temperature > 30 {
        println("It's hot!")
    } else {
        println("It's nice!")
    }
}
```

### If-Else If-Else

```sapphire
fn main() {
    let score = 85
    
    if score >= 90 {
        println("Grade: A")
    } else if score >= 80 {
        println("Grade: B")
    } else if score >= 70 {
        println("Grade: C")
    } else {
        println("Grade: F")
    }
}
```

## Comparison Operators

```sapphire
fn main() {
    let x = 10
    let y = 20
    
    println(x == y)  // Equal to: false
    println(x != y)  // Not equal to: true
    println(x < y)   // Less than: true
    println(x > y)   // Greater than: false
    println(x <= y)  // Less than or equal: true
    println(x >= y)  // Greater than or equal: false
}
```

## Logical Operators

Combine conditions:

```sapphire
fn main() {
    let age = 25
    let has_license = true
    
    // AND operator
    if age >= 18 and has_license {
        println("You can drive!")
    }
    
    // OR operator
    let is_weekend = true
    let is_holiday = false
    
    if is_weekend or is_holiday {
        println("No work today!")
    }
    
    // NOT operator
    let is_raining = false
    
    if not is_raining {
        println("Let's go outside!")
    }
}
```

## While Loops

Repeat code while a condition is true:

```sapphire
fn main() {
    let mut count = 1
    
    while count <= 5 {
        println("Count: " + str(count))
        count = count + 1
    }
}
```

Output:
```
Count: 1
Count: 2
Count: 3
Count: 4
Count: 5
```

### Infinite Loop with Break

```sapphire
fn main() {
    let mut i = 0
    
    while true {
        println(i)
        i = i + 1
        
        if i >= 5 {
            break  // Exit the loop
        }
    }
}
```

## For Loops

Iterate over a range:

```sapphire
fn main() {
    // Loop from 0 to 4
    for i in 0..5 {
        println(i)
    }
}
```

Output:
```
0
1
2
3
4
```

### Loop with Step

```sapphire
fn main() {
    // Count by 2s
    for i in 0..10 step 2 {
        println(i)
    }
}
```

Output:
```
0
2
4
6
8
```

### Loop Over Array

```sapphire
fn main() {
    let fruits = ["apple", "banana", "orange"]
    
    for fruit in fruits {
        println(fruit)
    }
}
```

## Continue Statement

Skip to the next iteration:

```sapphire
fn main() {
    for i in 0..10 {
        if i % 2 == 0 {
            continue  // Skip even numbers
        }
        println(i)  // Only prints odd numbers
    }
}
```

Output:
```
1
3
5
7
9
```

## Nested Loops

Loops inside loops:

```sapphire
fn main() {
    for i in 1..4 {
        for j in 1..4 {
            print(str(i) + "x" + str(j) + "=" + str(i*j) + "  ")
        }
        println("")
    }
}
```

Output:
```
1x1=1  1x2=2  1x3=3  
2x1=2  2x2=4  2x3=6  
3x1=3  3x2=6  3x3=9  
```

## Match Statement (Switch)

Pattern matching for multiple conditions:

```sapphire
fn main() {
    let day = "Monday"
    
    match day {
        "Monday" => println("Start of the week"),
        "Friday" => println("Almost weekend!"),
        "Saturday" | "Sunday" => println("Weekend!"),
        _ => println("Midweek day")
    }
}
```

## Practice Exercises

### Exercise 1: Even or Odd
Write a program that checks if a number is even or odd:

```sapphire
fn main() {
    let number = 7
    
    if number % 2 == 0 {
        println(str(number) + " is even")
    } else {
        println(str(number) + " is odd")
    }
}
```

### Exercise 2: Countdown
Write a countdown from 10 to 1:

```sapphire
fn main() {
    let mut count = 10
    
    while count > 0 {
        println(count)
        count = count - 1
    }
    println("Blast off!")
}
```

### Exercise 3: Multiplication Table
Print the multiplication table for a number:

```sapphire
fn main() {
    let number = 5
    
    for i in 1..11 {
        println(str(number) + " x " + str(i) + " = " + str(number * i))
    }
}
```

### Exercise 4: FizzBuzz
Classic programming challenge:

```sapphire
fn main() {
    for i in 1..101 {
        if i % 15 == 0 {
            println("FizzBuzz")
        } else if i % 3 == 0 {
            println("Fizz")
        } else if i % 5 == 0 {
            println("Buzz")
        } else {
            println(i)
        }
    }
}
```

### Exercise 5: Grade Calculator
Calculate letter grade from score:

```sapphire
fn main() {
    let score = 87
    let grade = ""
    
    if score >= 90 {
        grade = "A"
    } else if score >= 80 {
        grade = "B"
    } else if score >= 70 {
        grade = "C"
    } else if score >= 60 {
        grade = "D"
    } else {
        grade = "F"
    }
    
    println("Your grade is: " + grade)
}
```

## Common Mistakes

### 1. Forgetting Braces
```sapphire
// Wrong:
if x > 5
    println("Big")

// Correct:
if x > 5 {
    println("Big")
}
```

### 2. Using = Instead of ==
```sapphire
// Wrong:
if x = 5 {  // This assigns, not compares!
    println("Five")
}

// Correct:
if x == 5 {
    println("Five")
}
```

### 3. Infinite Loops
```sapphire
// Wrong:
let mut i = 0
while i < 10 {
    println(i)
    // Forgot to increment i!
}

// Correct:
let mut i = 0
while i < 10 {
    println(i)
    i = i + 1
}
```

## Summary

In this tutorial, you learned:
- ✅ If statements and conditions
- ✅ Comparison and logical operators
- ✅ While loops
- ✅ For loops
- ✅ Break and continue
- ✅ Match statements

## Next Steps

Continue to [Tutorial 3: Functions](03_FUNCTIONS.md) to learn about:
- Defining functions
- Parameters and return values
- Function scope

Happy coding! 🚀
