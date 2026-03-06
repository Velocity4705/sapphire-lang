# Tutorial 4: Arrays and Collections

Learn how to work with collections of data in Sapphire.

## Arrays

Arrays store multiple values of the same type:

```sapphire
fn main() {
    // Create an array
    let numbers = [1, 2, 3, 4, 5]
    let fruits = ["apple", "banana", "orange"]
    
    println(numbers)
    println(fruits)
}
```

### Accessing Elements

Use index (starting from 0):

```sapphire
fn main() {
    let fruits = ["apple", "banana", "orange"]
    
    println(fruits[0])  // apple
    println(fruits[1])  // banana
    println(fruits[2])  // orange
}
```

### Array Length

```sapphire
fn main() {
    let numbers = [10, 20, 30, 40, 50]
    
    println("Length: " + str(len(numbers)))  // 5
}
```

### Modifying Arrays

```sapphire
fn main() {
    let mut numbers = [1, 2, 3, 4, 5]
    
    numbers[0] = 10
    numbers[2] = 30
    
    println(numbers)  // [10, 2, 30, 4, 5]
}
```

### Iterating Over Arrays

```sapphire
fn main() {
    let fruits = ["apple", "banana", "orange", "grape"]
    
    // Method 1: For-each loop
    for fruit in fruits {
        println(fruit)
    }
    
    // Method 2: Index-based loop
    for i in 0..len(fruits) {
        println(str(i) + ": " + fruits[i])
    }
}
```

## Multi-dimensional Arrays

Arrays of arrays:

```sapphire
fn main() {
    // 2D array (matrix)
    let matrix = [
        [1, 2, 3],
        [4, 5, 6],
        [7, 8, 9]
    ]
    
    // Access elements
    println(matrix[0][0])  // 1
    println(matrix[1][2])  // 6
    println(matrix[2][1])  // 8
    
    // Iterate over 2D array
    for row in matrix {
        for value in row {
            print(str(value) + " ")
        }
        println("")
    }
}
```

## Array Operations

### Finding Elements

```sapphire
fn find(arr: [int], target: int) -> int {
    for i in 0..len(arr) {
        if arr[i] == target {
            return i
        }
    }
    return -1  // Not found
}

fn main() {
    let numbers = [10, 20, 30, 40, 50]
    
    let index = find(numbers, 30)
    if index != -1 {
        println("Found at index: " + str(index))
    } else {
        println("Not found")
    }
}
```

### Sum of Array

```sapphire
fn sum(arr: [int]) -> int {
    let mut total = 0
    for num in arr {
        total = total + num
    }
    return total
}

fn main() {
    let numbers = [1, 2, 3, 4, 5]
    println("Sum: " + str(sum(numbers)))  // 15
}
```

### Maximum Value

```sapphire
fn max(arr: [int]) -> int {
    let mut maximum = arr[0]
    for num in arr {
        if num > maximum {
            maximum = num
        }
    }
    return maximum
}

fn main() {
    let numbers = [45, 23, 67, 12, 89, 34]
    println("Max: " + str(max(numbers)))  // 89
}
```

### Reverse Array

```sapphire
fn reverse(arr: [int]) -> [int] {
    let mut result = []
    for i in len(arr)-1..0 step -1 {
        result.append(arr[i])
    }
    return result
}

fn main() {
    let numbers = [1, 2, 3, 4, 5]
    let reversed = reverse(numbers)
    println(reversed)  // [5, 4, 3, 2, 1]
}
```

## Lists (Dynamic Arrays)

Lists can grow and shrink:

```sapphire
fn main() {
    // Create empty list
    let mut items = []
    
    // Add elements
    items.append(10)
    items.append(20)
    items.append(30)
    
    println(items)  // [10, 20, 30]
    
    // Remove element
    items.remove(1)  // Remove at index 1
    println(items)  // [10, 30]
    
    // Insert element
    items.insert(1, 15)  // Insert 15 at index 1
    println(items)  // [10, 15, 30]
}
```

### List Methods

```sapphire
fn main() {
    let mut numbers = [1, 2, 3]
    
    // Append
    numbers.append(4)
    println(numbers)  // [1, 2, 3, 4]
    
    // Pop (remove last)
    let last = numbers.pop()
    println("Popped: " + str(last))  // 4
    println(numbers)  // [1, 2, 3]
    
    // Clear
    numbers.clear()
    println(numbers)  // []
    
    // Check if empty
    if numbers.is_empty() {
        println("List is empty")
    }
}
```

## Dictionaries (Hash Maps)

Store key-value pairs:

```sapphire
fn main() {
    // Create dictionary
    let mut ages = {
        "Alice": 25,
        "Bob": 30,
        "Charlie": 35
    }
    
    // Access values
    println(ages["Alice"])  // 25
    
    // Add new entry
    ages["David"] = 28
    
    // Update value
    ages["Alice"] = 26
    
    // Check if key exists
    if ages.has_key("Bob") {
        println("Bob's age: " + str(ages["Bob"]))
    }
    
    // Remove entry
    ages.remove("Charlie")
    
    // Iterate over dictionary
    for key in ages.keys() {
        println(key + ": " + str(ages[key]))
    }
}
```

### Dictionary Methods

```sapphire
fn main() {
    let mut scores = {
        "Math": 95,
        "English": 87,
        "Science": 92
    }
    
    // Get keys
    let subjects = scores.keys()
    println(subjects)
    
    // Get values
    let grades = scores.values()
    println(grades)
    
    // Get size
    println("Number of subjects: " + str(len(scores)))
    
    // Clear dictionary
    scores.clear()
}
```

## Sets

Store unique values:

```sapphire
fn main() {
    // Create set
    let mut numbers = {1, 2, 3, 4, 5}
    
    // Add element
    numbers.add(6)
    numbers.add(3)  // Duplicate, won't be added
    
    println(numbers)  // {1, 2, 3, 4, 5, 6}
    
    // Check membership
    if numbers.contains(3) {
        println("3 is in the set")
    }
    
    // Remove element
    numbers.remove(4)
    
    // Set operations
    let set1 = {1, 2, 3}
    let set2 = {3, 4, 5}
    
    let union = set1.union(set2)        // {1, 2, 3, 4, 5}
    let intersection = set1.intersect(set2)  // {3}
    let difference = set1.difference(set2)   // {1, 2}
}
```

## Slicing

Get a portion of an array:

```sapphire
fn main() {
    let numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    
    // Slice from index 2 to 5
    let slice1 = numbers[2..5]
    println(slice1)  // [2, 3, 4]
    
    // Slice from start to index 5
    let slice2 = numbers[..5]
    println(slice2)  // [0, 1, 2, 3, 4]
    
    // Slice from index 5 to end
    let slice3 = numbers[5..]
    println(slice3)  // [5, 6, 7, 8, 9]
    
    // Slice with step
    let slice4 = numbers[::2]  // Every 2nd element
    println(slice4)  // [0, 2, 4, 6, 8]
}
```

## Practice Exercises

### Exercise 1: Average Calculator
Calculate the average of numbers in an array:

```sapphire
fn average(numbers: [float]) -> float {
    let mut sum = 0.0
    for num in numbers {
        sum = sum + num
    }
    return sum / float(len(numbers))
}

fn main() {
    let grades = [85.5, 92.0, 78.5, 95.0, 88.0]
    let avg = average(grades)
    println("Average: " + str(avg))
}
```

### Exercise 2: Remove Duplicates
Remove duplicate values from an array:

```sapphire
fn remove_duplicates(arr: [int]) -> [int] {
    let mut result = []
    let mut seen = {}
    
    for num in arr {
        if not seen.contains(num) {
            result.append(num)
            seen.add(num)
        }
    }
    
    return result
}

fn main() {
    let numbers = [1, 2, 2, 3, 4, 3, 5, 1]
    let unique = remove_duplicates(numbers)
    println(unique)  // [1, 2, 3, 4, 5]
}
```

### Exercise 3: Word Counter
Count word frequency in a sentence:

```sapphire
fn count_words(text: str) -> {str: int} {
    let words = text.split(" ")
    let mut counts = {}
    
    for word in words {
        if counts.has_key(word) {
            counts[word] = counts[word] + 1
        } else {
            counts[word] = 1
        }
    }
    
    return counts
}

fn main() {
    let sentence = "the quick brown fox jumps over the lazy dog"
    let counts = count_words(sentence)
    
    for word in counts.keys() {
        println(word + ": " + str(counts[word]))
    }
}
```

### Exercise 4: Matrix Addition
Add two matrices:

```sapphire
fn add_matrices(a: [[int]], b: [[int]]) -> [[int]] {
    let mut result = []
    
    for i in 0..len(a) {
        let mut row = []
        for j in 0..len(a[0]) {
            row.append(a[i][j] + b[i][j])
        }
        result.append(row)
    }
    
    return result
}

fn main() {
    let matrix1 = [[1, 2], [3, 4]]
    let matrix2 = [[5, 6], [7, 8]]
    
    let sum = add_matrices(matrix1, matrix2)
    println(sum)  // [[6, 8], [10, 12]]
}
```

### Exercise 5: Shopping Cart
Implement a simple shopping cart:

```sapphire
fn main() {
    let mut cart = {}
    
    // Add items
    cart["Apple"] = 1.50
    cart["Banana"] = 0.75
    cart["Orange"] = 1.25
    
    // Calculate total
    let mut total = 0.0
    for item in cart.keys() {
        let price = cart[item]
        println(item + ": $" + str(price))
        total = total + price
    }
    
    println("Total: $" + str(total))
}
```

## Common Mistakes

### 1. Index Out of Bounds
```sapphire
// Wrong:
let arr = [1, 2, 3]
println(arr[5])  // Error! Index out of bounds

// Correct:
if 5 < len(arr) {
    println(arr[5])
} else {
    println("Index out of bounds")
}
```

### 2. Modifying Immutable Array
```sapphire
// Wrong:
let arr = [1, 2, 3]
arr[0] = 10  // Error! arr is immutable

// Correct:
let mut arr = [1, 2, 3]
arr[0] = 10  // OK
```

### 3. Wrong Type in Array
```sapphire
// Wrong:
let arr = [1, 2, "three"]  // Error! Mixed types

// Correct:
let arr = [1, 2, 3]  // All same type
```

## Summary

In this tutorial, you learned:
- ✅ Arrays and array operations
- ✅ Multi-dimensional arrays
- ✅ Lists (dynamic arrays)
- ✅ Dictionaries (hash maps)
- ✅ Sets
- ✅ Slicing

## Next Steps

Continue to [Tutorial 5: Strings](05_STRINGS.md) to learn about:
- String manipulation
- String methods
- Formatting
- Regular expressions

Happy coding! 🚀
