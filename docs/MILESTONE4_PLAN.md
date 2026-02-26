# Milestone 4: Memory Management - Implementation Plan

## Goal
Implement automatic memory management with garbage collection and memory safety features, making Sapphire safe and easy to use while maintaining high performance.

## Timeline
Months 10-12 (3 months)

## Overview

Memory management is critical for a modern language. We'll implement:
1. Automatic garbage collection (no manual malloc/free)
2. Memory safety (no use-after-free, no double-free)
3. Leak detection and profiling
4. Optional ownership system for performance-critical code

## Phases

### Phase 1: Memory Allocator (Week 1-2)
**Objective:** Custom memory allocator with tracking

#### Tasks
1. Design memory layout
   - Object header (type, size, GC flags)
   - Alignment requirements
   - Metadata storage

2. Implement allocator
   - Bump allocator for fast allocation
   - Free list for reuse
   - Size classes for efficiency
   - Thread-local allocation

3. Allocation tracking
   - Track all allocations
   - Record allocation site
   - Memory usage statistics

4. Integration with codegen
   - Replace malloc with custom allocator
   - Add allocation calls in IR
   - Track object lifetimes

#### Deliverables
- `runtime/allocator.h` - Allocator interface
- `runtime/allocator.cpp` - Allocator implementation
- `src/codegen/memory.cpp` - Memory codegen
- Allocation tracking working

#### Success Criteria
- Can allocate objects
- Allocation is fast (< 100ns)
- Memory usage tracked correctly
- No crashes or corruption

---

### Phase 2: Garbage Collector (Week 3-5)
**Objective:** Mark-and-sweep garbage collector

#### Tasks
1. GC Algorithm Design
   - Mark phase: trace reachable objects
   - Sweep phase: free unreachable objects
   - Root set: stack, globals, registers
   - Write barriers for incremental GC

2. Mark Phase Implementation
   - Stack scanning
   - Register scanning
   - Global variable scanning
   - Object graph traversal
   - Mark bits in object headers

3. Sweep Phase Implementation
   - Iterate over heap
   - Free unmarked objects
   - Coalesce free blocks
   - Update free lists

4. GC Triggers
   - Allocation threshold
   - Heap size threshold
   - Explicit GC calls
   - Idle time collection

5. Integration
   - Add GC calls to runtime
   - Safe points in generated code
   - Stack maps for precise GC

#### Deliverables
- `runtime/gc.h` - GC interface
- `runtime/gc.cpp` - GC implementation
- `runtime/roots.cpp` - Root scanning
- Working garbage collector

#### Success Criteria
- GC collects unreachable objects
- No memory leaks in test suite
- GC pause times < 10ms
- Memory usage stable

---

### Phase 3: Memory Safety (Week 6-7)
**Objective:** Prevent memory errors

#### Tasks
1. Use-After-Free Prevention
   - Null out freed pointers
   - Poison freed memory
   - Detect dangling pointers

2. Double-Free Prevention
   - Track freed objects
   - Assert on double-free
   - Clear free flags

3. Buffer Overflow Prevention
   - Bounds checking on arrays
   - Guard pages
   - Canaries

4. Type Safety
   - Runtime type information
   - Type checks on casts
   - Prevent type confusion

5. Safe Pointers
   - Null checks
   - Optional types
   - Smart pointer wrappers

#### Deliverables
- `runtime/safety.h` - Safety checks
- `runtime/safety.cpp` - Safety implementation
- Bounds checking in codegen
- Type safety checks

#### Success Criteria
- No use-after-free possible
- No double-free possible
- Buffer overflows caught
- Type errors caught at runtime

---

### Phase 4: Reference Counting (Week 8-9)
**Objective:** Reference counting for resources

#### Tasks
1. Reference Counter Design
   - Atomic reference counts
   - Increment/decrement operations
   - Zero-count detection

2. RC Implementation
   - RefCounted base class
   - Automatic inc/dec
   - Cycle detection

3. Smart Pointers
   - Rc<T> (reference counted)
   - Arc<T> (atomic reference counted)
   - Weak<T> (weak references)

4. Integration
   - Use RC for file handles
   - Use RC for network sockets
   - Use RC for large objects

5. Cycle Breaking
   - Weak references
   - Cycle detector
   - Manual cycle breaking

#### Deliverables
- `runtime/refcount.h` - RC interface
- `runtime/refcount.cpp` - RC implementation
- Smart pointer types
- Cycle detection

#### Success Criteria
- RC works correctly
- No reference leaks
- Cycles detected and broken
- Performance overhead < 5%

---

### Phase 5: Ownership System (Week 10-11)
**Objective:** Optional ownership for zero-cost abstractions

#### Tasks
1. Ownership Semantics
   - Move semantics
   - Borrow checking
   - Lifetime tracking

2. Syntax Design
   - owned keyword
   - borrowed keyword
   - Lifetime annotations

3. Static Analysis
   - Ownership checker
   - Borrow checker
   - Lifetime inference

4. Codegen Integration
   - No-op for owned types
   - Assertions for borrowed types
   - Lifetime validation

5. Documentation
   - Ownership guide
   - Best practices
   - Examples

#### Deliverables
- `src/semantic/ownership.h` - Ownership checker
- `src/semantic/ownership.cpp` - Implementation
- Ownership syntax
- Documentation

#### Success Criteria
- Ownership prevents use-after-free
- Borrow checker prevents data races
- Zero runtime overhead
- Clear error messages

---

### Phase 6: Memory Profiler (Week 12)
**Objective:** Tools for memory debugging

#### Tasks
1. Profiler Design
   - Allocation tracking
   - Leak detection
   - Memory usage graphs

2. Profiler Implementation
   - Hook allocations
   - Record stack traces
   - Generate reports

3. Leak Detector
   - Find unreachable objects
   - Report allocation sites
   - Suggest fixes

4. Visualization
   - Memory usage over time
   - Allocation hotspots
   - Object lifetimes

5. Integration
   - Command-line tool
   - Runtime API
   - IDE integration

#### Deliverables
- `tools/memprof.cpp` - Memory profiler
- Leak detection tool
- Visualization scripts
- Documentation

#### Success Criteria
- Can detect leaks
- Shows allocation sites
- Visualizations are clear
- Easy to use

---

## Technical Architecture

### Memory Layout

```
Object Header (16 bytes):
+------------------+
| Type ID (8 bytes)|
+------------------+
| Size (4 bytes)   |
+------------------+
| GC Flags (4 bytes)|
+------------------+
| Object Data...   |
```

### GC Algorithm

```
Mark Phase:
1. Start with root set (stack, globals)
2. Mark each root object
3. Recursively mark referenced objects
4. Use work queue to avoid stack overflow

Sweep Phase:
1. Iterate over all allocated objects
2. If unmarked, add to free list
3. If marked, clear mark bit
4. Coalesce adjacent free blocks
```

### Allocation Strategy

```
Small objects (< 256 bytes):
- Size classes (8, 16, 32, 64, 128, 256)
- Thread-local free lists
- Fast path: bump pointer

Large objects (>= 256 bytes):
- Direct allocation
- Separate large object space
- Lazy sweeping
```

### Reference Counting

```cpp
template<typename T>
class Rc {
    T* ptr;
    size_t* refcount;
    
    Rc(T* p) : ptr(p), refcount(new size_t(1)) {}
    Rc(const Rc& other) : ptr(other.ptr), refcount(other.refcount) {
        (*refcount)++;
    }
    ~Rc() {
        if (--(*refcount) == 0) {
            delete ptr;
            delete refcount;
        }
    }
};
```

### Ownership System

```sapphire
# Owned value (moved)
fn take_ownership(owned x: String) {
    # x is moved here, caller can't use it
}

# Borrowed value (reference)
fn borrow(borrowed x: String) {
    # x is borrowed, caller still owns it
}

# Usage
let s = "hello"
borrow(s)  # OK, s is borrowed
take_ownership(s)  # OK, s is moved
# print(s)  # ERROR: s was moved
```

## Performance Targets

### Allocation Performance
- Small object allocation: < 100ns
- Large object allocation: < 1μs
- Thread-local allocation: < 50ns

### GC Performance
- Pause time: < 10ms (99th percentile)
- Throughput overhead: < 5%
- Memory overhead: < 20%

### Memory Usage
- Comparable to Go
- Better than Java
- Worse than C++ (but safer)

### Comparison

| Language | Allocation | GC Pause | Memory Overhead |
|----------|-----------|----------|-----------------|
| C++      | 50ns      | N/A      | 0%              |
| Rust     | 50ns      | N/A      | 0%              |
| Go       | 100ns     | 1-5ms    | 10-20%          |
| Java     | 100ns     | 10-50ms  | 30-50%          |
| Sapphire | 100ns     | < 10ms   | 10-20%          |

## Testing Strategy

### Unit Tests
- Allocator correctness
- GC correctness
- Safety checks
- Reference counting

### Integration Tests
- Memory-intensive programs
- Concurrent allocation
- GC under load
- Leak detection

### Stress Tests
- Allocate millions of objects
- Trigger GC repeatedly
- Concurrent GC
- Memory pressure

### Benchmarks
- Allocation speed
- GC pause times
- Memory usage
- Throughput

## Dependencies

### Required Libraries
- None (custom implementation)

### Optional Libraries
- libunwind (stack unwinding)
- valgrind (memory debugging)
- perf (profiling)

## Success Criteria

### Must Have
✅ Automatic garbage collection
✅ No memory leaks in test suite
✅ GC pause times < 10ms
✅ Memory safety (no use-after-free)
✅ Reference counting for resources

### Should Have
✅ Ownership system working
✅ Memory profiler functional
✅ Leak detection accurate
✅ Performance overhead < 5%

### Nice to Have
✅ Incremental GC
✅ Concurrent GC
✅ Generational GC
✅ Compacting GC

## Risks and Mitigations

### Risk 1: GC Pauses Too Long
**Mitigation:** Incremental GC, concurrent marking, smaller heap sizes

### Risk 2: Memory Overhead Too High
**Mitigation:** Compact object headers, efficient free lists, lazy sweeping

### Risk 3: Ownership System Too Complex
**Mitigation:** Make it optional, provide good defaults, clear error messages

### Risk 4: Performance Regression
**Mitigation:** Benchmark continuously, optimize hot paths, profile regularly

## Next Steps After Milestone 4

1. **Milestone 5:** Concurrency (async/await, goroutines)
2. **Milestone 6:** Standard library expansion
3. **Milestone 7:** Developer tooling (LSP, debugger)

## Documentation Updates

- [ ] Memory management guide
- [ ] GC tuning guide
- [ ] Ownership system guide
- [ ] Memory profiler guide
- [ ] Best practices

## Timeline Summary

| Week | Phase | Deliverable |
|------|-------|-------------|
| 1-2  | Memory Allocator | Custom allocator |
| 3-5  | Garbage Collector | Mark-and-sweep GC |
| 6-7  | Memory Safety | Safety checks |
| 8-9  | Reference Counting | RC and smart pointers |
| 10-11| Ownership System | Optional ownership |
| 12   | Memory Profiler | Profiling tools |

**Total:** 12 weeks (3 months)

## Conclusion

Milestone 4 will make Sapphire safe and easy to use while maintaining high performance. By the end, we'll have:

- Automatic memory management (no manual malloc/free)
- Memory safety (no use-after-free, no leaks)
- Optional ownership for zero-cost abstractions
- Tools for memory debugging and profiling

This will make Sapphire competitive with Go and Rust while being easier to use than C++!

Let's build the safest fast language! 🛡️🚀

