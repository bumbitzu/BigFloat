# BigFloat

BigFloat is a C++ library for high-precision floating-point arithmetic. It provides arithmetic operators, stream support, and tools for working with very large numbers.

## Building

The repository includes a Visual Studio solution (`BigFloat.sln`). You can build the project using Visual Studio or compile the sources manually with a C++17 compiler:

```bash
g++ BigFloat/BigFloat.cpp BigFloat/Main.cpp -std=c++17 -o BigFloatApp
```

## Usage

`Main.cpp` contains a simple example that approximates π using the `BigFloat` class. Use `set_precision` to control the number of digits of precision before performing operations.

