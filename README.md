# BigFloat – Arbitrary‑Precision Floating‑Point Library in C++

## Overview

BigFloat is a fully custom, high‑precision floating‑point numeric class written in C++ without external multiprecision libraries. It supports:

* Arbitrary‑precision addition, subtraction, multiplication, division
* Decimal exponent model with block‑based integer storage
* Fast‑path optimizations when exponents match
* Full decimal formatting, trimming, and precision control
* Verified correctness through unit testing
* Performance benchmarking for real‑world workloads

This library is intended both as a learning project and as a portfolio piece demonstrating deep understanding of numerical systems, data representation, performance engineering, and complex algorithmic design.

---

## Internal Representation

Each BigFloat stores a value in the form:

```
value = sign * ( Σ digits[i] * BASE^i ) * 10^exponent
```

Where:

* `BASE = 10^9` (fits in 32‑bit integer)
* `digits` = little‑endian vector of base‑1e9 chunks
* `exponent` = decimal exponent (power of 10)
* `is_negative` = sign flag
* `precision_limit` = output decimal precision cap

### ASCII Architecture Diagram

```
BIGFLOAT OBJECT LAYOUT
======================

  ┌───────────────────────────┐
  │ BigFloat                  │
  ├───────────────────────────┤
  │ bool is_negative          │  → true / false
  │ int  exponent             │  → decimal exponent
  │ int  precision_limit      │  → max decimals printed
  │ vector<int> digits        │  → base‑1e9 blocks (little endian)
  └───────────────────────────┘

DIGIT BLOCK STRUCTURE
---------------------
 digits vector (little‑endian):

    index →   [0]         [1]         [2]        ... [N]
              ┌────────┐ ┌────────┐ ┌────────┐      
              │chunk0  │ │chunk1  │ │chunk2  │
              └────────┘ └────────┘ └────────┘
               10^0       10^9       10^18

NUMERIC VALUE RECONSTRUCTION
----------------------------

    IntegerPart = chunk0
                 + chunk1 * BASE
                 + chunk2 * BASE^2
                 + ...

    FinalValue = sign * IntegerPart * 10^(exponent)
```

### Example: Parsing "1.001"

```
Input string       → "1.001"
Strip decimal      → "1001"
Stored digits      → [1001]
Stored exponent    → -3
Final numeric value = 1001 * 10^-3 = 1.001
```

### Example: Adding numbers with different exponents

```
a = 1        → digits=[1], exponent=0
b = 0.001    → digits=[1], exponent=-3

common_exp = -3
Scale a by 10^3 → digits=[1000]
Add digits      → [1000] + [1] = [1001]
Result          → 1001 * 10^-3 = 1.001
```

---

## Implemented Operations

### Addition

* Fast path for equal exponents (pure BigInt behavior)
* Slow path scales operands to a shared exponent
* Handles sign differences via reduction to subtraction

### Subtraction

* Handles sign differences (a - (-b) → a + b)
* Aligns exponents when needed
* Subtracts by magnitude and assigns correct sign

### Multiplication

* Full convolution‑style block multiplication
* Exponents add normally

### Division

* Long‑division algorithm with precision scaling
* Internal search to determine each quotient block
* Exponent adjusted after digit extension

### Output Formatting

* Reconstructs integer string from digit blocks
* Places decimal point using exponent
* Trims trailing zeros
* Removes trailing decimal point
* Respects `precision_limit`

---

## Testing

Unit tests are located in `Test.cpp` and cover:

* Constructors
* Addition
* Subtraction
* Multiplication
* Division
* Precision trimming
* Special cases (zero, negative numbers)
* Stress test: factorial(100)

Example test (from `Test.cpp`):

```
assert_equals(BigFloat("10.5") + BigFloat("0.5"), "11");
assert_equals(BigFloat("1") - BigFloat("0.001"), "0.999");
assert_equals(BigFloat("355") / BigFloat("113"), "3.141592");
```

All tests run automatically when executing `main`.

---

## Benchmarks

Benchmarks are implemented in `Benchmark.cpp`.

### Results

| Operation         | Output Size       | Time    |
| ----------------- | ----------------- | ------- |
| factorial(5000)   | ~158 digits       | ~240 ms |
| fibonacci(50,000) | ~10k‑digit number | ~2.0 s  |
| 2^10000           | ~3011 digits      | ~95 ms  |
| π 10,000 decimals | via division      | ~64 ms  |

These highlight:

* Optimized BigInt‑mode arithmetic when exponents match
* Good performance relative to pure educational implementations
* Real‑world computational capability

---

## File Structure

```
BigFloat.h        → class definition
BigFloat.cpp      → full implementation
Test.h/.cpp       → unit tests
Benchmark.h/.cpp  → performance tests
Main.cpp          → entry point
```

---


## Future Extensions

* Karatsuba / FFT multiplication
* Newton–Raphson division for faster reciprocals
* Support for transcendental functions (exp, log, sin, cos)
* Additional rounding modes
* Configurable BASE representation (1e4 / 1e9 / 2^32)
* SIMD‑optimized addition/subtraction

---

## License

MIT License
