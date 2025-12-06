#include "Benchmark.h"
#include "BigFloat.h"
#include <iostream>

using namespace std;
using namespace std::chrono;

// Timer RAII for benchmarking
class Timer {
    high_resolution_clock::time_point start;
    string name;
public:
    Timer(string n) : name(n) {
        start = high_resolution_clock::now();
        cout << ">>> START: " << name << "..." << endl;
    }
    ~Timer() {
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << ">>> DONE: " << name << " in " << duration.count() << " ms" << endl << endl;
    }
};

// Benchmark: Calculate 5000!
void benchmark_factorial() {
    
    Timer t("Factorial 5000");

    BigFloat fact("1");
    for (int i = 1; i <= 5000; ++i) {
        fact = fact * BigFloat(i);
    }
    cout << "   Calculation complete." << endl;
}

// Benchmark: Calculate Fibonacci(50000)
void benchmark_fibonacci() {
    
    Timer t("Fibonacci 50.000");

    BigFloat a("0");
    BigFloat b("1");
    BigFloat c;

    for (int i = 2; i <= 50000; ++i) {
        c = a + b;
        a = b;
        b = c;
    }
    cout << "   Calculation complete." << endl;
}

// Benchmark: Calculate 2^10000
void benchmark_power() {
    
    Timer t("Power 2^10000");

    BigFloat base("2");
    BigFloat res("1");

    // Naive exponentiation
    for (int i = 0; i < 10000; ++i) {
        res = res * base;
    }
    cout << "   Calculation complete." << endl;
}

// Benchmark: Calculate PI to 10,000 decimal places
void benchmark_pi() {

    Timer t("PI with 10.000 Decimals");

    BigFloat num("1000000"); // Circumference
    BigFloat den("318309");  // Diameter


    BigFloat pi;
    set_precision(num, den, 10000);

    pi = num / den;

    cout << "   Calculation complete." << endl;

};