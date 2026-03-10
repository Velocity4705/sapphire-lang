# Data Science Quick Start Guide

**Get started with data science in Sapphire in 5 minutes!**

---

## Run the Demos

### Interactive Launcher
```bash
./run_data_science.sh
```

Choose from:
1. Statistical Analysis (Sales Data)
2. Machine Learning (Linear Regression)
3. Time Series Analysis (Stock Forecasting)
4. Run all demos

### Individual Demos
```bash
# Statistical Analysis
./data_science_demo

# Machine Learning
./ml_simple

# Time Series
./time_series_simple
```

---

## Example: Statistical Analysis

```sapphire
import system

// Your data
let sales_jan = 45
let sales_feb = 52
let sales_mar = 48
// ... more data

// Calculate mean
let sum = sales_jan + sales_feb + sales_mar
let mean = sum / 3
print("Mean:", mean)

// Calculate variance
let diff1 = sales_jan - mean
let diff2 = sales_feb - mean
let diff3 = sales_mar - mean
let variance = (diff1 * diff1 + diff2 * diff2 + diff3 * diff3) / 3
print("Variance:", variance)
```

---

## Example: Machine Learning

```sapphire
import system

// Training data
let x1 = 15  // House size
let x2 = 20
let x3 = 25

let y1 = 30  // Price
let y2 = 38
let y3 = 48

// Calculate means
let mean_x = (x1 + x2 + x3) / 3
let mean_y = (y1 + y2 + y3) / 3

// Calculate slope (m) and intercept (b)
let dx1 = x1 - mean_x
let dx2 = x2 - mean_x
let dx3 = x3 - mean_x

let dy1 = y1 - mean_y
let dy2 = y2 - mean_y
let dy3 = y3 - mean_y

let cov = (dx1 * dy1 + dx2 * dy2 + dx3 * dy3) / 3
let var_x = (dx1 * dx1 + dx2 * dx2 + dx3 * dx3) / 3

let m = cov / var_x
let b = mean_y - (m * mean_x)

print("Slope:", m)
print("Intercept:", b)

// Make prediction
let test_size = 30
let prediction = (m * test_size) + b
print("Predicted price:", prediction)
```

---

## Example: Time Series

```sapphire
import system

// Stock prices
let day1 = 100
let day2 = 102
let day3 = 105
let day4 = 107
let day5 = 110

// Calculate return
let total_return = day5 - day1
let return_pct = (total_return * 100) / day1
print("Return:", return_pct, "%")

// Moving average
let ma3 = (day3 + day4 + day5) / 3
print("3-day MA:", ma3)

// Forecast
let slope = (day5 - day1) / 4
let forecast = day5 + slope
print("Next day forecast:", forecast)
```

---

## Compilation

### Quick Compile
```bash
./sapp compile myprogram.spp 2>/dev/null | grep -A 1000 "^; ModuleID" > myprogram.ll
llc myprogram.ll -filetype=obj -o myprogram.o
gcc myprogram.o stdlib/system/system.cpp -o myprogram -lstdc++ -lpthread
./myprogram
```

### One-liner
```bash
./sapp compile myprogram.spp 2>/dev/null | grep -A 1000 "^; ModuleID" > myprogram.ll && llc myprogram.ll -filetype=obj -o myprogram.o && gcc myprogram.o stdlib/system/system.cpp -o myprogram -lstdc++ -lpthread && ./myprogram
```

---

## Common Patterns

### Calculate Mean
```sapphire
let sum = val1 + val2 + val3 + val4
let mean = sum / 4
```

### Calculate Variance
```sapphire
let d1 = val1 - mean
let d2 = val2 - mean
let d3 = val3 - mean
let variance = (d1 * d1 + d2 * d2 + d3 * d3) / 3
```

### Calculate Correlation
```sapphire
let cov = (dx1 * dy1 + dx2 * dy2 + dx3 * dy3) / n
let var_x = (dx1 * dx1 + dx2 * dx2 + dx3 * dx3) / n
let var_y = (dy1 * dy1 + dy2 * dy2 + dy3 * dy3) / n
// correlation = cov / sqrt(var_x * var_y)
```

### Moving Average
```sapphire
let ma3 = (val1 + val2 + val3) / 3
let ma5 = (val1 + val2 + val3 + val4 + val5) / 5
```

### Linear Regression
```sapphire
// y = mx + b
let m = covariance / variance_x
let b = mean_y - (m * mean_x)
let prediction = (m * x_new) + b
```

---

## Performance Tips

### Use Integer Math When Possible
```sapphire
// Faster
let result = (a * 100) / b  // Percentage as integer

// Slower (if using floats in future)
let result = (a / b) * 100.0
```

### Minimize Loops
```sapphire
// Better: Unroll small loops
let sum = val1 + val2 + val3 + val4

// Avoid: Loop for small datasets
let i = 0
while i < 4:
    sum = sum + values[i]
    i = i + 1
```

### Batch Operations
```sapphire
// Calculate all statistics in one pass
let sum = 0
let sum_sq = 0
// ... accumulate
let mean = sum / n
let variance = (sum_sq / n) - (mean * mean)
```

---

## Available Functions

### System Library
```sapphire
import system

// Timing
sapphire_system_timestamp_ms()  // Milliseconds
sapphire_system_timestamp_us()  // Microseconds
sapphire_system_timestamp_ns()  // Nanoseconds
sapphire_system_sleep_ms(ms)    // Sleep

// Memory
sapphire_system_allocate(size)  // Allocate
sapphire_system_free(ptr)       // Free

// System Info
sapphire_system_cpu_count()     // CPU cores
sapphire_system_cpu_arch()      // Architecture
```

---

## Next Steps

### Learn More
- Read `DATA_SCIENCE_COMPLETE.md` for comprehensive guide
- Check `DATA_SCIENCE_ROADMAP.md` for future features
- See `LEVEL5_DATA_SCIENCE_SUMMARY.md` for session summary

### Try These Projects
1. Analyze your own CSV data
2. Build a price prediction model
3. Forecast time series data
4. Create a recommendation system
5. Implement clustering algorithm

### Contribute
- Implement new algorithms
- Add more examples
- Improve documentation
- Share your projects

---

## Troubleshooting

### Compilation Fails
```bash
# Check LLVM IR was generated
ls -lh myprogram.ll

# Check for errors
./sapp compile myprogram.spp
```

### Runtime Errors
```bash
# Add debug prints
print("Debug: value =", value)

# Check variable values
print("x =", x, "y =", y)
```

### Performance Issues
```bash
# Measure execution time
let start = sapphire_system_timestamp_us()
// ... your code
let end = sapphire_system_timestamp_us()
print("Time:", end - start, "us")
```

---

## Resources

### Documentation
- `DATA_SCIENCE_COMPLETE.md` - Complete guide
- `DATA_SCIENCE_ROADMAP.md` - Future plans
- `SYSTEM_LIBRARY_COMPLETE.md` - System functions

### Examples
- `data_science_demo.spp` - Statistical analysis
- `ml_simple.spp` - Machine learning
- `time_series_simple.spp` - Time series

### Community
- GitHub: [sapphire-lang](https://github.com/velo4705/sapphire-lang)
- Issues: Report bugs and request features
- Discussions: Ask questions

---

## Quick Reference Card

| Task | Function | Example |
|------|----------|---------|
| Mean | `sum / n` | `mean = (a + b + c) / 3` |
| Variance | `sum_sq / n - mean²` | `var = sq_sum / n` |
| Std Dev | `sqrt(variance)` | Approximate with loop |
| Correlation | `cov / (std_x * std_y)` | `corr = cov / var_x` |
| Linear Reg | `y = mx + b` | `pred = m * x + b` |
| Moving Avg | `sum / window` | `ma = (a + b + c) / 3` |
| Forecast | `last + slope` | `next = val + trend` |

---

**Happy data science with Sapphire!** 📊💎

