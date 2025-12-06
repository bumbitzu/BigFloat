#include "Benchmark.h"
#include "Test.h"
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;


int main()
{
    /// Unit Tests
	cout << "========================================" << endl;
    cout << "      BigFloat Optimization Tests       " << endl;
    cout << "========================================" << endl;

    try 
    {
        test_constructors();
        test_addition();
        test_subtraction();
        test_multiplication();
        test_division();
        test_factorial_stress();
    }
    catch (const char* msg) 
    {
        cout << "CRITICAL ERROR: Uncaught exception: " << msg << endl;
    }
    catch (const std::exception& e) 
    {
        cout << "CRITICAL ERROR: Standard exception: " << e.what() << endl;
    }

    /// Speed Benchmarks

	cout << "========================================" << endl;
	cout << "       BIGFLOAT SPEED BENCHMARK         " << endl;
	cout << "========================================" << endl;

	try {
		benchmark_factorial();
		benchmark_fibonacci();
		benchmark_power();
		benchmark_pi();
	}
	catch (const std::exception& e) {
		cout << "Error: " << e.what() << endl;
	}

	// find pi
    //int precision = 100000; // Set desired precision
    //BigFloat num("355");
    //BigFloat den("113");
    //set_precision(num, den, precision);

    //high_resolution_clock::time_point start = high_resolution_clock::now();
    //BigFloat pi = num / den;
    //high_resolution_clock::time_point end = high_resolution_clock::now();

    //cout << "PI Approximation (355/113) with precision " << precision << ":\n";
    //cout << pi << endl;

    //auto duration = duration_cast<milliseconds>(end - start);
    //cout << "Calculation took " << duration.count() << " ms\n";

}
