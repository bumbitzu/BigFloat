#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

class BigFloat
{
private:
    // digits are stored little-endian in base 1e9 for compactness
    std::vector<uint32_t> digits;
    bool negativ;
    bool is_float;
    int float_point;     // number of digits before the decimal point (for I/O)
    int float_endpoint;  // number of digits after the decimal point (for I/O)
    int precision;       // requested precision for division

    void normalize() noexcept;
    static int compare_abs(const BigFloat& lhs, const BigFloat& rhs) noexcept;
    static BigFloat add_abs(const BigFloat& lhs, const BigFloat& rhs) noexcept;
    static BigFloat sub_abs(const BigFloat& lhs, const BigFloat& rhs) noexcept;
    static BigFloat mul_abs(const BigFloat& lhs, const BigFloat& rhs) noexcept;
    static BigFloat div_abs(const BigFloat& lhs, const BigFloat& rhs, int target_scale);
    static void align_scale(BigFloat& lhs, BigFloat& rhs) noexcept;

public:
    BigFloat();
    explicit BigFloat(long long);
    explicit BigFloat(const std::string&);
    BigFloat(const BigFloat&);
    BigFloat(BigFloat&&) noexcept = default;

    BigFloat& operator=(const BigFloat&);
    BigFloat& operator=(BigFloat&&) noexcept = default;

    friend bool E0(const BigFloat&);
    friend BigFloat Balance_Point(std::vector<int>&, std::vector<int>&, const BigFloat&, const BigFloat&);

    friend std::istream& operator>>(std::istream&, BigFloat&);
    friend std::ostream& operator<<(std::ostream&, const BigFloat&);

    friend BigFloat operator+(const BigFloat&, const BigFloat&);
    friend BigFloat operator+=(BigFloat&, const BigFloat&);

    friend BigFloat operator-(const BigFloat&, const BigFloat&);
    friend BigFloat operator-=(BigFloat&, const BigFloat&);

    friend BigFloat operator*(const BigFloat&, const BigFloat&);
    friend BigFloat operator*=(BigFloat&, const BigFloat&);

    friend BigFloat operator/(const BigFloat&, const BigFloat&);
    friend BigFloat operator/=(BigFloat&, const BigFloat&);

    friend BigFloat operator%(const BigFloat&, const BigFloat&);
    friend BigFloat operator%=(BigFloat&, const BigFloat&);

    BigFloat& operator++();
    BigFloat operator++(int);
    BigFloat& operator--();
    BigFloat operator--(int);

    friend bool operator==(const BigFloat&, const BigFloat&);
    friend bool operator!=(const BigFloat&, const BigFloat&);
    friend bool operator<(const BigFloat&, const BigFloat&);
    friend bool operator<=(const BigFloat&, const BigFloat&);
    friend bool operator>(const BigFloat&, const BigFloat&);
    friend bool operator>=(const BigFloat&, const BigFloat&);

    int operator[](int) const;
    friend void set_precision(BigFloat&, BigFloat&, const int&);
};

