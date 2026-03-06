# Tutorial 5: Strings

Learn how to work with text in Sapphire.

## String Basics

Strings are sequences of characters:

```sapphire
fn main() {
    let message = "Hello, World!"
    let name = "Alice"
    let quote = 'Single quotes work too'
    
    println(message)
    println(name)
    println(quote)
}
```

### Multi-line Strings

```sapphire
fn main() {
    let poem = """
    Roses are red,
    Violets are blue,
    Sapphire is fast,
    And easy too!
    """
    
    println(poem)
}
```

## String Concatenation

Combine strings:

```sapphire
fn main() {
    let first = "Hello"
    let second = "World"
    
    // Using +
    let greeting = first + ", " + second + "!"
    println(greeting)  // Hello, World!
    
    // Multiple concatenations
    let name = "Alice"
    let age = 25
    let message = "My name is " + name + " and I am " + str(age) + " years old"
    println(message)
}
```

## String Length

```sapphire
fn main() {
    let text = "Hello, World!"
    
    println("Length: " + str(len(text)))  // 13
    
    let empty = ""
    println("Empty length: " + str(len(empty)))  // 0
}
```

## Accessing Characters

```sapphire
fn main() {
    let word = "Sapphire"
    
    // Access by index
    println(word[0])  // S
    println(word[1])  // a
    println(word[7])  // e
    
    // Last character
    println(word[len(word) - 1])  // e
}
```

## String Methods

### Case Conversion

```sapphire
fn main() {
    let text = "Hello, World!"
    
    println(text.to_upper())  // HELLO, WORLD!
    println(text.to_lower())  // hello, world!
    println(text.to_title())  // Hello, World!
}
```

### Trimming Whitespace

```sapphire
fn main() {
    let text = "   Hello, World!   "
    
    println(text.trim())       // "Hello, World!"
    println(text.trim_left())  // "Hello, World!   "
    println(text.trim_right()) // "   Hello, World!"
}
```

### Splitting Strings

```sapphire
fn main() {
    let sentence = "The quick brown fox"
    let words = sentence.split(" ")
    
    for word in words {
        println(word)
    }
    // Output:
    // The
    // quick
    // brown
    // fox
    
    // Split by comma
    let csv = "apple,banana,orange"
    let fruits = csv.split(",")
    println(fruits)  // ["apple", "banana", "orange"]
}
```

### Joining Strings

```sapphire
fn main() {
    let words = ["Hello", "World", "from", "Sapphire"]
    
    let sentence = words.join(" ")
    println(sentence)  // Hello World from Sapphire
    
    let csv = words.join(",")
    println(csv)  // Hello,World,from,Sapphire
}
```

### Replacing Text

```sapphire
fn main() {
    let text = "Hello, World!"
    
    let new_text = text.replace("World", "Sapphire")
    println(new_text)  // Hello, Sapphire!
    
    // Replace all occurrences
    let repeated = "foo bar foo baz foo"
    let replaced = repeated.replace("foo", "qux")
    println(replaced)  // qux bar qux baz qux
}
```

### Checking Content

```sapphire
fn main() {
    let text = "Hello, World!"
    
    // Starts with
    if text.starts_with("Hello") {
        println("Starts with Hello")
    }
    
    // Ends with
    if text.ends_with("!") {
        println("Ends with !")
    }
    
    // Contains
    if text.contains("World") {
        println("Contains World")
    }
}
```

### Finding Substrings

```sapphire
fn main() {
    let text = "Hello, World!"
    
    // Find first occurrence
    let index = text.find("World")
    if index != -1 {
        println("Found at index: " + str(index))  // 7
    }
    
    // Find last occurrence
    let repeated = "foo bar foo baz"
    let last_index = repeated.rfind("foo")
    println("Last occurrence at: " + str(last_index))  // 8
}
```

## String Slicing

Extract parts of a string:

```sapphire
fn main() {
    let text = "Hello, World!"
    
    // Slice from index 0 to 5
    println(text[0..5])  // Hello
    
    // Slice from index 7 to end
    println(text[7..])  // World!
    
    // Slice from start to index 5
    println(text[..5])  // Hello
    
    // Get last 6 characters
    println(text[len(text)-6..])  // World!
}
```

## String Formatting

### Basic Formatting

```sapphire
fn main() {
    let name = "Alice"
    let age = 25
    
    // Using concatenation
    let msg1 = "Name: " + name + ", Age: " + str(age)
    println(msg1)
    
    // Using format function
    let msg2 = format("Name: {}, Age: {}", name, age)
    println(msg2)
}
```

### Format Specifiers

```sapphire
fn main() {
    let pi = 3.14159
    
    // Decimal places
    println(format("{:.2}", pi))  // 3.14
    println(format("{:.4}", pi))  // 3.1416
    
    // Padding
    let num = 42
    println(format("{:5}", num))   // "   42"
    println(format("{:05}", num))  // "00042"
}
```

## String Comparison

```sapphire
fn main() {
    let str1 = "apple"
    let str2 = "banana"
    let str3 = "apple"
    
    // Equality
    println(str1 == str3)  // true
    println(str1 == str2)  // false
    
    // Inequality
    println(str1 != str2)  // true
    
    // Lexicographic comparison
    println(str1 < str2)   // true (apple comes before banana)
    println(str2 > str1)   // true
}
```

## Character Operations

### Checking Character Types

```sapphire
fn main() {
    let ch = 'A'
    
    if ch.is_alpha() {
        println("Is alphabetic")
    }
    
    if ch.is_digit() {
        println("Is digit")
    }
    
    if ch.is_upper() {
        println("Is uppercase")
    }
    
    if ch.is_lower() {
        println("Is lowercase")
    }
    
    if ch.is_whitespace() {
        println("Is whitespace")
    }
}
```

### Character Conversion

```sapphire
fn main() {
    let ch = 'a'
    
    println(ch.to_upper())  // A
    println(ch.to_lower())  // a
    
    // ASCII value
    println(ch.to_ascii())  // 97
    
    // From ASCII
    let new_ch = char(65)
    println(new_ch)  // A
}
```

## String Building

Efficiently build strings:

```sapphire
fn main() {
    let mut builder = StringBuilder()
    
    builder.append("Hello")
    builder.append(", ")
    builder.append("World")
    builder.append("!")
    
    let result = builder.to_string()
    println(result)  // Hello, World!
}
```

## Practice Exercises

### Exercise 1: Reverse String
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

### Exercise 2: Palindrome Checker
Check if a string is a palindrome:

```sapphire
fn is_palindrome(s: str) -> bool {
    let cleaned = s.to_lower().replace(" ", "")
    let reversed = reverse(cleaned)
    return cleaned == reversed
}

fn main() {
    println(is_palindrome("racecar"))      // true
    println(is_palindrome("hello"))        // false
    println(is_palindrome("A man a plan a canal Panama"))  // true
}
```

### Exercise 3: Word Count
Count words in a string:

```sapphire
fn word_count(text: str) -> int {
    let words = text.trim().split(" ")
    let mut count = 0
    
    for word in words {
        if len(word) > 0 {
            count = count + 1
        }
    }
    
    return count
}

fn main() {
    let text = "The quick brown fox jumps over the lazy dog"
    println("Word count: " + str(word_count(text)))  // 9
}
```

### Exercise 4: Title Case
Convert string to title case:

```sapphire
fn title_case(s: str) -> str {
    let words = s.to_lower().split(" ")
    let mut result = []
    
    for word in words {
        if len(word) > 0 {
            let first = word[0].to_upper()
            let rest = word[1..]
            result.append(first + rest)
        }
    }
    
    return result.join(" ")
}

fn main() {
    println(title_case("hello world"))           // Hello World
    println(title_case("the quick brown fox"))   // The Quick Brown Fox
}
```

### Exercise 5: Caesar Cipher
Implement a simple Caesar cipher:

```sapphire
fn caesar_cipher(text: str, shift: int) -> str {
    let mut result = ""
    
    for ch in text {
        if ch.is_alpha() {
            let base = if ch.is_upper() { 'A' } else { 'a' }
            let offset = (ch.to_ascii() - base.to_ascii() + shift) % 26
            result = result + char(base.to_ascii() + offset)
        } else {
            result = result + ch
        }
    }
    
    return result
}

fn main() {
    let text = "Hello, World!"
    let encrypted = caesar_cipher(text, 3)
    println("Encrypted: " + encrypted)  // Khoor, Zruog!
    
    let decrypted = caesar_cipher(encrypted, -3)
    println("Decrypted: " + decrypted)  // Hello, World!
}
```

### Exercise 6: Email Validator
Simple email validation:

```sapphire
fn is_valid_email(email: str) -> bool {
    // Check for @ symbol
    if not email.contains("@") {
        return false
    }
    
    let parts = email.split("@")
    if len(parts) != 2 {
        return false
    }
    
    let username = parts[0]
    let domain = parts[1]
    
    // Check username and domain are not empty
    if len(username) == 0 or len(domain) == 0 {
        return false
    }
    
    // Check domain has a dot
    if not domain.contains(".") {
        return false
    }
    
    return true
}

fn main() {
    println(is_valid_email("user@example.com"))    // true
    println(is_valid_email("invalid.email"))       // false
    println(is_valid_email("@example.com"))        // false
}
```

## Common Mistakes

### 1. String Immutability
```sapphire
// Wrong:
let text = "Hello"
text[0] = 'h'  // Error! Strings are immutable

// Correct:
let text = "Hello"
let new_text = 'h' + text[1..]
println(new_text)  // hello
```

### 2. Forgetting to Convert to String
```sapphire
// Wrong:
let age = 25
let message = "I am " + age + " years old"  // Error!

// Correct:
let age = 25
let message = "I am " + str(age) + " years old"
```

### 3. Index Out of Bounds
```sapphire
// Wrong:
let text = "Hi"
println(text[5])  // Error! Index out of bounds

// Correct:
let text = "Hi"
if 5 < len(text) {
    println(text[5])
} else {
    println("Index out of bounds")
}
```

## Summary

In this tutorial, you learned:
- ✅ String basics and concatenation
- ✅ String methods (case, trim, split, join, replace)
- ✅ String slicing and formatting
- ✅ Character operations
- ✅ String comparison

## Next Steps

Continue to [Tutorial 6: File I/O](06_FILE_IO.md) to learn about:
- Reading files
- Writing files
- File operations
- Working with paths

Happy coding! 🚀
