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
void BigFloat::scaleByDecimals(int dec) { // dec >= 0
    if (dec == 0 || (digits.size() == 1 && digits[0] == 0)) return;

    int chunk_shift = dec / BASE_DIGITS;
    int digit_shift = dec % BASE_DIGITS;

    // 1) shift pe chunk-uri (multiplicare cu BASE^chunk_shift)
    if (chunk_shift > 0) {
        digits.insert(digits.begin(), chunk_shift, 0);
    }

    // 2) shift pe zecimale (multiplicare cu 10^digit_shift)
    static const int POW10[9] = { 1,10,100,1000,10000,100000,1000000,10000000,100000000 };

    if (digit_shift > 0) {
        long long mul = POW10[digit_shift];
        long long carry = 0;
        for (size_t i = 0; i < digits.size(); ++i) {
            long long val = digits[i] * mul + carry;
            digits[i] = (int)(val % BASE);
            carry = val / BASE;
        }
        while (carry) {
            digits.push_back((int)(carry % BASE));
            carry /= BASE;
        }
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
    if (a.exponent == b.exponent) {
        if (a.digits.size() > b.digits.size()) return 1;
        if (a.digits.size() < b.digits.size()) return -1;
        for (int i = (int)a.digits.size() - 1; i >= 0; --i) {
            if (a.digits[i] > b.digits[i]) return 1;
            if (a.digits[i] < b.digits[i]) return -1;
        }
        return 0;
    }
    // 0 vs 0
    if (a.digits.size() == 1 && a.digits[0] == 0 &&
        b.digits.size() == 1 && b.digits[0] == 0) {
        return 0;
    }

    // Compare by "magnitude" first
    auto magnitude = [](const BigFloat& x) -> long long {
        if (x.digits.size() == 1 && x.digits[0] == 0) return LLONG_MIN; 

        // Calculate number of digits in absolute value
        int ms = x.digits.back(); 
        int ms_digits = 0;
        while (ms > 0) {
            ms_digits++;
            ms /= 10;
        }
        if (ms_digits == 0) ms_digits = 1; 

        
        long long digits_count = (long long)(x.digits.size() - 1) * BASE_DIGITS + ms_digits;
        return digits_count + x.exponent; 
        };

    // Get magnitudes
    long long magA = magnitude(a);
    long long magB = magnitude(b);

    if (magA > magB) return 1;
    if (magA < magB) return -1;

    // Slow-path: align exponents and compare digit by digit
    BigFloat A = a;
    BigFloat B = b;

    int common_exp = std::min(A.exponent, B.exponent);

    if (A.exponent > common_exp) {
        A.scaleByDecimals(A.exponent - common_exp);
        A.exponent = common_exp;
    }
    if (B.exponent > common_exp) {
        B.scaleByDecimals(B.exponent - common_exp);
        B.exponent = common_exp;
    }

    // Now compare digit by digit
    A.trim();
    B.trim();

    if (A.digits.size() > B.digits.size()) return 1;
    if (A.digits.size() < B.digits.size()) return -1;

    for (int i = (int)A.digits.size() - 1; i >= 0; --i) {
        if (A.digits[i] > B.digits[i]) return 1;
        if (A.digits[i] < B.digits[i]) return -1;
    }
    return 0;
}


// --- Arithmetic Operators ---
// Addition
BigFloat BigFloat::operator+(const BigFloat& other) const {

    // Case: different signs => reduce to subtraction
    if (is_negative != other.is_negative) {
        BigFloat a = *this;
        BigFloat b = other;
        if (a.is_negative) {
            a.is_negative = false;
            return b - a;         
        }
        else {
            b.is_negative = false;
            return a - b;         
        }
    }

    // From here: both have the same sign
    if (exponent == other.exponent) {
        BigFloat res;
        res.is_negative = is_negative;
        res.exponent = exponent;
        res.precision_limit = std::max(precision_limit, other.precision_limit);

        const auto& v1 = digits;
        const auto& v2 = other.digits;

        size_t n1 = v1.size(), n2 = v2.size();
        size_t maxSize = std::max(n1, n2);

        res.digits.reserve(maxSize + 1);

        long long carry = 0;
        for (size_t i = 0; i < maxSize || carry; ++i) {
            long long sum = carry;
            if (i < n1) sum += v1[i];
            if (i < n2) sum += v2[i];

            res.digits.push_back((int)(sum % BASE));
            carry = sum / BASE;
        }

        res.trim();
        return res;
    }

    // Slow-path
    BigFloat a = *this;
    BigFloat b = other;

    BigFloat res;
    res.is_negative = is_negative;
    res.precision_limit = std::max(precision_limit, other.precision_limit);

    int common_exp = std::min(a.exponent, b.exponent);

    // Align a and b to common_exp
    if (a.exponent > common_exp) {
        a.scaleByDecimals(a.exponent - common_exp); 
        a.exponent = common_exp;
    }

    if (b.exponent > common_exp) {
        b.scaleByDecimals(b.exponent - common_exp);
        b.exponent = common_exp;
    }

    res.exponent = common_exp;

    // Now perform addition
    size_t n1 = a.digits.size();
    size_t n2 = b.digits.size();
    size_t maxSize = std::max(n1, n2);

    res.digits.reserve(maxSize + 1);

    long long carry = 0;
    for (size_t i = 0; i < maxSize || carry; ++i) {
        long long sum = carry;
        if (i < n1) sum += a.digits[i];
        if (i < n2) sum += b.digits[i];

        res.digits.push_back((int)(sum % BASE));
        carry = sum / BASE;
    }

    res.trim();
    return res;
}

// Subtraction
BigFloat BigFloat::operator-(const BigFloat& other) const {

    // Case: different signs => reduce to addition
    if (is_negative != other.is_negative) {
        BigFloat a = *this;
        BigFloat b = other;
        
        if (b.is_negative) {

            b.is_negative = false;
            return a + b;
        }
        else {

            a.is_negative = false;
            BigFloat res = a + b;  
            res.is_negative = true;  
            res.trim();              
            return res;
        }
    }
    // From here: both have the same sign
    BigFloat a = *this;
    BigFloat b = other;

    // Fast-path: equal exponents => BigInt - BigInt
    BigFloat res;
    res.precision_limit = std::max(precision_limit, other.precision_limit);

    int common_exp = std::min(a.exponent, b.exponent);

    // Align a to common_exp
    if (a.exponent > common_exp) {
        a.scaleByDecimals(a.exponent - common_exp);
        a.exponent = common_exp;
    }
    if (b.exponent > common_exp) {
        b.scaleByDecimals(b.exponent - common_exp);
        b.exponent = common_exp;
    }

    res.exponent = common_exp;

    // Determine which absolute value is greater
    a.trim();
    b.trim();

    bool a_ge_b;  

    // Compare absolute values
    if (a.digits.size() != b.digits.size()) {
        a_ge_b = (a.digits.size() > b.digits.size());
    }
    else {
        int i = (int)a.digits.size() - 1;
        for (; i >= 0 && a.digits[i] == b.digits[i]; --i) {}
        if (i < 0) {

            a_ge_b = true;
        }
        else {
            a_ge_b = (a.digits[i] > b.digits[i]);
        }
    }

    const std::vector<int>& v_big = a_ge_b ? a.digits : b.digits;
    const std::vector<int>& v_small = a_ge_b ? b.digits : a.digits;

    if (a_ge_b) {
        res.is_negative = is_negative;
    }
    else {
        res.is_negative = !is_negative;
    }


    res.digits.clear();
    res.digits.reserve(v_big.size());

    long long borrow = 0;
    for (size_t i = 0; i < v_big.size(); ++i) {
        long long sub = (long long)v_big[i] - borrow - (i < v_small.size() ? v_small[i] : 0);
        if (sub < 0) {
            sub += BASE;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        res.digits.push_back((int)sub);
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