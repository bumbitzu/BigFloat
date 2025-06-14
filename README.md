# BigFloat

BigFloat is a simple C++ library that implements arbitrary-precision
floating point arithmetic using base 10 digits. It supports common
arithmetic and comparison operators so you can work with very large or
very precise numbers.

## Building

The repository contains a Visual Studio solution (`BigFloat.sln`) for
Windows users. You can also build the project with any C++17 compliant
compiler. For example with `g++`:

```bash
g++ -std=c++17 BigFloat/BigFloat.cpp BigFloat/Main.cpp -o BigFloat
```

Running the resulting executable will compute an approximation of π using
`BigFloat` operations.

## Usage

Create `BigFloat` objects from integers or strings and use the usual
operators:

```cpp
BigFloat a("123.45");
BigFloat b(10);
BigFloat c = a * b + 42;
```

Precision for division can be specified with `set_precision` before
performing the operation.

See `Main.cpp` for a short example that calculates π.

## Repository structure

- `BigFloat/BigFloat.h` – class definition
- `BigFloat/BigFloat.cpp` – implementation
- `BigFloat/Main.cpp` – example program
- `BigFloat.sln` and project files – Visual Studio configuration
