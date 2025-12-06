#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

class BigFloat
{
private:
    // Constatnts for optimization
    static const int BASE = 1000000000; // Base 10^9
    static const int BASE_DIGITS = 9;   // Digits per element

    // Digits stored in little-endian order (least significant chunk first)
    std::vector<int> digits;

    // Exponent to represent the decimal point position
    int exponent;

    // Sign of the number
    bool is_negative;

    // Precision limit for decimal places
    int precision_limit;

    // Helper functions
    void trim();
    void scaleByDecimals(int dec);
    static int compareAbs(const BigFloat& a, const BigFloat& b); 
    static void divide_by_int(BigFloat& a, int b); 

public:

    BigFloat();
    BigFloat(long long n);
    BigFloat(const std::string& s);

    // Set precision for decimal places
    void setPrecision(int p);

    // Arithmetic operators
    BigFloat operator+(const BigFloat& other) const;
    BigFloat operator-(const BigFloat& other) const;
    BigFloat operator*(const BigFloat& other) const;
    BigFloat operator/(const BigFloat& other) const;

    // Compound assignment operators
    BigFloat& operator+=(const BigFloat& other);
    BigFloat& operator-=(const BigFloat& other);
    BigFloat& operator*=(const BigFloat& other);
    BigFloat& operator/=(const BigFloat& other);

    // Comparison operators
    bool operator==(const BigFloat& other) const;
    bool operator!=(const BigFloat& other) const;
    bool operator<(const BigFloat& other) const;
    bool operator>(const BigFloat& other) const;
    bool operator<=(const BigFloat& other) const;
    bool operator>=(const BigFloat& other) const;

    // Output operator
    friend std::ostream& operator<<(std::ostream& os, const BigFloat& bf);
    friend void set_precision(BigFloat&, BigFloat&, const int&);
};