#include "BigFloat.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cmath>

using namespace std;

// --- Constructors ---
BigFloat::BigFloat() {
    is_negative = false;
    exponent = 0;
    precision_limit = 50; // Default precision
}

BigFloat::BigFloat(long long n) : BigFloat() {
    if (n == 0) {
        digits.push_back(0);
        return;
    }
    if (n < 0) {
        is_negative = true;
        n = -n;
    }
    while (n > 0) {
        digits.push_back(n % BASE);
        n /= BASE;
    }
}

BigFloat::BigFloat(const std::string& s) : BigFloat() {
    if (s.empty()) { digits.push_back(0); return; }

    string str = s;
    int decimal_pos = -1;

 
    if (str[0] == '-') {
        is_negative = true;
        str.erase(0, 1);
    }
    else if (str[0] == '+') {
        str.erase(0, 1);
    }


    size_t found = str.find('.');
    if (found != string::npos) {
        decimal_pos = found;
        str.erase(found, 1); 

        exponent = -((int)str.size() - (int)found);
    }

    for (int i = (int)str.size(); i > 0; i -= BASE_DIGITS) {
        if (i < BASE_DIGITS) {
            digits.push_back(stoi(str.substr(0, i)));
        }
        else {
            digits.push_back(stoi(str.substr(i - BASE_DIGITS, BASE_DIGITS)));
        }
    }
    trim();
}

// --- Helper Functions ---

void BigFloat::trim() {

    // Remove leading zeros
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    // Handle zero case
    if (digits.size() == 1 && digits[0] == 0) {
        is_negative = false;
        exponent = 0;
    }
}
// --- Precision Management ---
void BigFloat::setPrecision(int p) {
    precision_limit = p;
}

// --- Static Helper Functions ---
void set_precision(BigFloat& ob1, BigFloat& ob2, const int& n) {
    ob1.setPrecision(n);
    ob2.setPrecision(n);
}
// Compare absolute values of two BigFloat numbers
int BigFloat::compareAbs(const BigFloat& a, const BigFloat& b) 
{

    if (a.digits.size() > b.digits.size()) return 1;
    if (a.digits.size() < b.digits.size()) return -1;
    for (int i = a.digits.size() - 1; i >= 0; i--) {
        if (a.digits[i] > b.digits[i]) return 1;
        if (a.digits[i] < b.digits[i]) return -1;
    }
    return 0;
}

// --- Arithmetic Operators ---
// Addition
BigFloat BigFloat::operator+(const BigFloat& other) const {
    if (is_negative == other.is_negative) {

        BigFloat res;
        res.is_negative = is_negative;
        res.exponent = std::min(exponent, other.exponent);
        res.precision_limit = std::max(precision_limit, other.precision_limit);

        int exp_diff = abs(exponent - other.exponent);

        std::vector<int> v1 = digits;
        std::vector<int> v2 = other.digits;

        int carry = 0;
        size_t n1 = v1.size(), n2 = v2.size();
        size_t maxSize = std::max(n1, n2);

        for (size_t i = 0; i < maxSize || carry; ++i) {
            long long sum = carry + (i < n1 ? v1[i] : 0) + (i < n2 ? v2[i] : 0);
            if (i < res.digits.size()) res.digits[i] = sum % BASE;
            else res.digits.push_back(sum % BASE);
            carry = sum / BASE;
        }
        return res;
    }
    else {

        BigFloat a = *this; a.is_negative = false;
        BigFloat b = other; b.is_negative = false;
        if (a > b) {
            BigFloat res = a - b;
            res.is_negative = is_negative; 
            return res;
        }
        else {
            BigFloat res = b - a;
            res.is_negative = other.is_negative;
            return res;
        }
    }
}
// Subtraction
BigFloat BigFloat::operator-(const BigFloat& other) const {
    if (is_negative != other.is_negative) {

        BigFloat b = other;
        b.is_negative = !b.is_negative;
        return *this + b;
    }

    if (compareAbs(*this, other) < 0) 
    {
        BigFloat res = other - *this;
        res.is_negative = !is_negative;
        return res;
    }

    BigFloat res;
    res.is_negative = is_negative;
    res.exponent = exponent; 

    int borrow = 0;
    for (size_t i = 0; i < digits.size(); ++i) {
        long long sub = digits[i] - borrow - (i < other.digits.size() ? other.digits[i] : 0);
        if (sub < 0) {
            sub += BASE;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        res.digits.push_back(sub);
    }
    res.trim();
    return res;
}
// Multiplication
BigFloat BigFloat::operator*(const BigFloat& other) const {
    BigFloat res;
    res.is_negative = (is_negative != other.is_negative);
    res.exponent = exponent + other.exponent;
    res.precision_limit = std::max(precision_limit, other.precision_limit);
    // Initialize result digits
    res.digits.resize(digits.size() + other.digits.size(), 0);
    // Multiply
    for (size_t i = 0; i < digits.size(); ++i) {
        long long carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry; ++j) {
            long long cur = res.digits[i + j] +
                digits[i] * 1LL * (j < other.digits.size() ? other.digits[j] : 0) +
                carry;
            res.digits[i + j] = cur % BASE;
            carry = cur / BASE;
        }
    }
    res.trim();
    return res;
}
// Division
BigFloat BigFloat::operator/(const BigFloat& other) const {
    if (other.digits.size() == 1 && other.digits[0] == 0) throw std::invalid_argument("Division by zero");

    // Prepare dividend and divisor
    BigFloat A = *this;
    BigFloat B = other;

    // Determine sign of the result
    bool result_neg = (A.is_negative != B.is_negative);
    A.is_negative = false;
    B.is_negative = false;

    // Adjust exponent for precision
    int final_exponent = A.exponent - B.exponent;

    // Add extra digits to A for precision
    int needed_chunks = (precision_limit / 9) + 2;

    // Append needed_chunks of zeros to A
    std::vector<int> new_digits(needed_chunks, 0);
    new_digits.insert(new_digits.end(), A.digits.begin(), A.digits.end());
    A.digits = new_digits;

    // Update final exponent
    final_exponent -= (needed_chunks * 9);

    // Initialize quotient and remainder
    BigFloat quotient;
    quotient.digits.resize(A.digits.size() - B.digits.size() + 1, 0);

    // Long division algorithm
    BigFloat remainder;
    remainder.exponent = 0;

    // Process each digit from most significant to least significant
    for (int i = A.digits.size() - 1; i >= 0; i--) 
    {
        // Shift remainder left and add next digit
        remainder.digits.insert(remainder.digits.begin(), A.digits[i]);
        remainder.trim();

        // Binary search for the quotient digit
        int left = 0, right = BASE - 1;
        int q = 0;

        // Find the maximum q such that B * q <= remainder
        while (left <= right) {
            int mid = left + (right - left) / 2;
            BigFloat attempt = B;
            long long carry = 0;
            for (int& d : attempt.digits) {
                long long val = d * 1LL * mid + carry;
                d = val % BASE;
                carry = val / BASE;
            }
            if (carry) attempt.digits.push_back(carry);
            attempt.trim();

            if (attempt <= remainder) {
                q = mid;
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }

        // Set the found quotient digit
        quotient.digits.push_back(q);

        // Subtract B * q from remainder
        BigFloat to_subtract = B;
        long long carry = 0;
        for (int& d : to_subtract.digits) {
            long long val = d * 1LL * q + carry;
            d = val % BASE;
            carry = val / BASE;
        }
        if (carry) to_subtract.digits.push_back(carry);
        to_subtract.trim();

        // Update remainder
        remainder = remainder - to_subtract;
    }
    // Reverse quotient digits
    std::reverse(quotient.digits.begin(), quotient.digits.end());

    // Remove leading zeros from quotient
    quotient.is_negative = result_neg;
    quotient.exponent = final_exponent;
    quotient.trim();

    return quotient;
}

// --- Compound Assignment Operators ---
BigFloat& BigFloat::operator+=(const BigFloat& other) { *this = *this + other; return *this; }
BigFloat& BigFloat::operator-=(const BigFloat& other) { *this = *this - other; return *this; }
BigFloat& BigFloat::operator*=(const BigFloat& other) { *this = *this * other; return *this; }
BigFloat& BigFloat::operator/=(const BigFloat& other) { *this = *this / other; return *this; }

// --- Comparison Operators ---
// Equal
bool BigFloat::operator==(const BigFloat& other) const {
    return digits == other.digits && exponent == other.exponent && is_negative == other.is_negative;
}
// Not equal
bool BigFloat::operator!=(const BigFloat& other) const { return !(*this == other); }
bool BigFloat::operator<(const BigFloat& other) const {
    if (is_negative != other.is_negative) return is_negative; 
    if (is_negative) return compareAbs(*this, other) > 0;
    return compareAbs(*this, other) < 0;
}
// Greater than
bool BigFloat::operator>(const BigFloat& other) const { return other < *this; }
bool BigFloat::operator<=(const BigFloat& other) const { return !(*this > other); }
bool BigFloat::operator>=(const BigFloat& other) const { return !(*this < other); }

// --- Output Operator ---
std::ostream& operator<<(std::ostream& os, const BigFloat& bf) {
    if (bf.digits.empty() || (bf.digits.size() == 1 && bf.digits[0] == 0)) {
        os << "0";
        return os;
    }
    // Handle sign
    if (bf.is_negative) os << "-";

    stringstream ss;

    // Output most significant digit without leading zeros
    ss << bf.digits.back();

    // Output remaining digits with leading zeros
    for (int i = bf.digits.size() - 2; i >= 0; i--) {
        ss << setfill('0') << setw(9) << bf.digits[i];
    }

    // Construct the full number string
    string s = ss.str();
    int total_exp = bf.exponent;

    // Insert decimal point based on exponent
    if (total_exp >= 0) {
        os << s;
        for (int k = 0; k < total_exp; k++) os << '0';
    }
    else {

        // Decimal point position
        int len = s.length();
        int point_pos = len + total_exp; 

        // If point is before the first digit
        if (point_pos <= 0) {
            os << "0.";
            for (int k = 0; k < -point_pos; k++) os << '0';
            os << s;
        }
        else {
            os << s.substr(0, point_pos) << "." << s.substr(point_pos);
        }
    }

    return os;
}