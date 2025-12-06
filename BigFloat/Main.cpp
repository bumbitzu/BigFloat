#include "Benchmark.h"
#include "Test.h"
#include <iostream>
using namespace std;


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
	
	
}
