#include "Test.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void assert_equals(const BigFloat& result, const string& expected, const string& test_name) {
    stringstream ss;
    ss << result;
    string actual = ss.str();

    if (actual == expected) {
        cout << "[PASS] " << test_name << endl;
    }
    else {
        cout << "[FAIL] " << test_name << endl;
        cout << "       Expected: " << expected << endl;
        cout << "       Actual:   " << actual << endl;
    }
}

void assert_true(bool condition, const string& test_name) {
    if (condition) {
        cout << "[PASS] " << test_name << endl;
    }
    else {
        cout << "[FAIL] " << test_name << " (Expected true, got false)" << endl;
    }
}

void test_constructors() {
    cout << "\n--- Constructor Tests ---\n";
    assert_equals(BigFloat("123"), "123", "String Constructor Integer");
    assert_equals(BigFloat("123.456"), "123.456", "String Constructor Float");
    assert_equals(BigFloat("-500"), "-500", "Negative String");
    assert_equals(BigFloat(100), "100", "Long Long Constructor");
    assert_equals(BigFloat(0), "0", "Zero Constructor");
    assert_equals(BigFloat("0.005"), "0.005", "Small Float");
}

void test_addition() {
    cout << "\n--- Addition Tests ---\n";
    assert_equals(BigFloat("100") + BigFloat("200"), "300", "100 + 200");
    assert_equals(BigFloat("10.5") + BigFloat("0.5"), "11", "10.5 + 0.5 (Decimal logic)");
    assert_equals(BigFloat("100") + BigFloat("-50"), "50", "100 + (-50)");
    assert_equals(BigFloat("-50") + BigFloat("-50"), "-100", "-50 + (-50)");
    assert_equals(BigFloat("0.001") + BigFloat("0.002"), "0.003", "Small decimals");
}

void test_subtraction() {
    cout << "\n--- Subtraction Tests ---\n";
    assert_equals(BigFloat("100") - BigFloat("50"), "50", "100 - 50");
    assert_equals(BigFloat("50") - BigFloat("100"), "-50", "50 - 100 (Result Negative)");
    assert_equals(BigFloat("10.5") - BigFloat("0.5"), "10", "10.5 - 0.5");
    assert_equals(BigFloat("10") - BigFloat("-5"), "15", "10 - (-5) -> Addition");
}

void test_multiplication() {
    cout << "\n--- Multiplication Tests ---\n";
    assert_equals(BigFloat("12") * BigFloat("12"), "144", "12 * 12");
    assert_equals(BigFloat("0.5") * BigFloat("0.5"), "0.25", "0.5 * 0.5 (Decimals)");
    assert_equals(BigFloat("-2") * BigFloat("50"), "-100", "Negative * Positive");
    assert_equals(BigFloat("-2") * BigFloat("-2"), "4", "Negative * Negative");
    assert_equals(BigFloat("1000000") * BigFloat("0"), "0", "Multiply by Zero");

    BigFloat b1("1000000000");
    assert_equals(b1 * b1, "1000000000000000000", "Base Crossing (10^18)");
}

void test_division() {
    cout << "\n--- Division Tests ---\n";

    BigFloat a("100");
    BigFloat b("2");
    
    set_precision(a, b, 2);
    assert_equals(a / b, "50", "100 / 2");

    BigFloat c("1");
    BigFloat d("2");
    set_precision(c, d, 10);
    assert_equals(c / d, "0.5", "1 / 2");

    BigFloat num("355");
    BigFloat den("113");
    BigFloat pi;
    set_precision(num, den, 6);
    pi = num / den;
    assert_equals(pi, "3.141592", "PI Approximation (355/113)");

    bool caught = false;
    try {
        BigFloat x("10");
        BigFloat z("0");
        BigFloat res = x / z;
    }
    catch (const std::exception& e) {
        caught = true;
    }
    assert_true(caught, "Division by Zero Exception");
}

void test_factorial_stress() {
    cout << "\n--- Stress Test (Factorial 100) ---\n";

    BigFloat fact = 1;
    for (int i = 1; i <= 100; ++i) {
        fact = fact * BigFloat(i);
    }

    stringstream ss;
    ss << fact;
    string s = ss.str();

    cout << "100! calculated. Digits: " << s.length() << endl;
    assert_true(s.length() > 150, "Factorial 100 length check");
    assert_true(s.substr(s.length() - 5) == "00000", "Factorial trailing zeros check");

    assert_true(s.substr(0, 5) == "93326", "Factorial 100 value check (starts with 93326)");
}
