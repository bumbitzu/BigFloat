#include "BigFloat.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <stdexcept>
#include <utility>

namespace
{
    constexpr uint32_t kBase = 1000000000u;
    constexpr int kBaseDigits = 9;

    constexpr uint32_t pow10_small(int exp) noexcept
    {
        uint32_t value = 1;
        while (exp-- > 0)
        {
            value *= 10u;
        }
        return value;
    }

    int count_digits(uint32_t value) noexcept
    {
        int digits = 1;
        while (value >= 10)
        {
            value /= 10;
            ++digits;
        }
        return digits;
    }
}

BigFloat::BigFloat() : digits{0}, negativ(false), is_float(false), float_point(1), float_endpoint(0), precision(0) {}

BigFloat::BigFloat(long long value) : digits(), negativ(value < 0), is_float(false), float_point(0), float_endpoint(0), precision(0)
{
    unsigned long long abs_value = static_cast<unsigned long long>(negativ ? -value : value);
    if (abs_value == 0)
    {
        digits.push_back(0);
        float_point = 1;
        return;
    }

    while (abs_value > 0)
    {
        digits.push_back(static_cast<uint32_t>(abs_value % kBase));
        abs_value /= kBase;
    }
    normalize();
}

BigFloat::BigFloat(const std::string& str) : digits(), negativ(false), is_float(false), float_point(0), float_endpoint(0), precision(0)
{
    if (str.empty())
    {
        digits.push_back(0);
        float_point = 1;
        return;
    }

    std::string s = str;
    if (s.front() == '+')
    {
        s.erase(s.begin());
    }
    else if (s.front() == '-')
    {
        negativ = true;
        s.erase(s.begin());
    }

    const auto dot_pos = s.find('.');
    if (dot_pos != std::string::npos)
    {
        is_float = true;
        float_endpoint = static_cast<int>(s.size() - dot_pos - 1);
        s.erase(dot_pos, 1);
    }

    auto first_digit = s.find_first_not_of('0');
    if (first_digit == std::string::npos)
    {
        digits.push_back(0);
        negativ = false;
        is_float = false;
        float_endpoint = 0;
        float_point = 1;
        return;
    }
    s.erase(0, first_digit);

    digits.push_back(0);
    for (char c : s)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            throw std::invalid_argument("The string of digits contains unaccepted characters");
        }
        uint32_t carry = static_cast<uint32_t>(c - '0');
        for (auto& limb : digits)
        {
            uint64_t cur = static_cast<uint64_t>(limb) * 10u + carry;
            limb = static_cast<uint32_t>(cur % kBase);
            carry = static_cast<uint32_t>(cur / kBase);
        }
        if (carry)
        {
            digits.push_back(carry);
        }
    }

    normalize();
}

BigFloat::BigFloat(const BigFloat& other) = default;

bool E0(const BigFloat& ob)
{
    return ob.digits.size() == 1 && ob.digits.front() == 0;
}

BigFloat Balance_Point(std::vector<int>&, std::vector<int>&, const BigFloat& ob1, const BigFloat& ob2)
{
    BigFloat tmp = ob1;
    BigFloat tmp2 = ob2;
    BigFloat::align_scale(tmp, tmp2);
    return tmp;
}

BigFloat& BigFloat::operator=(const BigFloat& other)
{
    if (this != &other)
    {
        digits = other.digits;
        negativ = other.negativ;
        is_float = other.is_float;
        float_point = other.float_point;
        float_endpoint = other.float_endpoint;
        precision = other.precision;
    }
    return *this;
}

std::istream& operator>>(std::istream& in, BigFloat& ob)
{
    std::string str;
    in >> str;
    ob = BigFloat(str);
    return in;
}

static std::string to_decimal_string(const BigFloat& ob)
{
    std::string buffer;
    buffer.reserve(kBaseDigits * ob.digits.size());
    buffer.append(std::to_string(ob.digits.back()));
    for (int i = static_cast<int>(ob.digits.size()) - 2; i >= 0; --i)
    {
        auto chunk = std::to_string(ob.digits[static_cast<size_t>(i)]);
        buffer.append(std::string(kBaseDigits - static_cast<int>(chunk.size()), '0'));
        buffer.append(chunk);
    }
    return buffer;
}

std::ostream& operator<<(std::ostream& out, const BigFloat& ob)
{
    std::string buffer = to_decimal_string(ob);

    if (ob.is_float && ob.float_endpoint > 0)
    {
        if (static_cast<int>(buffer.size()) <= ob.float_endpoint)
        {
            buffer.insert(0, static_cast<size_t>(ob.float_endpoint - buffer.size() + 1), '0');
        }
        const auto pos = buffer.size() - static_cast<size_t>(ob.float_endpoint);
        buffer.insert(pos, 1, '.');
    }

    if (ob.negativ && !E0(ob))
    {
        buffer.insert(buffer.begin(), '-');
    }

    out << buffer;
    return out;
}

void BigFloat::normalize() noexcept
{
    while (digits.size() > 1 && digits.back() == 0)
    {
        digits.pop_back();
    }

    if (digits.size() == 1 && digits.front() == 0)
    {
        negativ = false;
        is_float = false;
        float_endpoint = 0;
        float_point = 1;
        return;
    }

    int decimal_digits = static_cast<int>((digits.size() - 1) * kBaseDigits + count_digits(digits.back()));
    float_point = std::max(1, decimal_digits - float_endpoint);
    is_float = float_endpoint > 0;
}

int BigFloat::compare_abs(const BigFloat& lhs, const BigFloat& rhs) noexcept
{
    if (lhs.float_endpoint != rhs.float_endpoint)
    {
        BigFloat a = lhs;
        BigFloat b = rhs;
        align_scale(a, b);
        return compare_abs(a, b);
    }

    if (lhs.digits.size() != rhs.digits.size())
    {
        return lhs.digits.size() < rhs.digits.size() ? -1 : 1;
    }
    for (int i = static_cast<int>(lhs.digits.size()) - 1; i >= 0; --i)
    {
        if (lhs.digits[static_cast<size_t>(i)] != rhs.digits[static_cast<size_t>(i)])
        {
            return lhs.digits[static_cast<size_t>(i)] < rhs.digits[static_cast<size_t>(i)] ? -1 : 1;
        }
    }
    return 0;
}

void BigFloat::align_scale(BigFloat& lhs, BigFloat& rhs) noexcept
{
    if (lhs.float_endpoint == rhs.float_endpoint)
    {
        return;
    }
    BigFloat* smaller = &lhs;
    int delta = rhs.float_endpoint - lhs.float_endpoint;
    if (delta < 0)
    {
        delta = -delta;
        smaller = &rhs;
    }

    int limb_shift = delta / kBaseDigits;
    int digit_shift = delta % kBaseDigits;

    if (limb_shift > 0)
    {
        smaller->digits.insert(smaller->digits.begin(), static_cast<size_t>(limb_shift), 0);
    }
    if (digit_shift > 0)
    {
        uint32_t mul = pow10_small(digit_shift);
        uint64_t carry = 0;
        for (auto& limb : smaller->digits)
        {
            uint64_t cur = static_cast<uint64_t>(limb) * mul + carry;
            limb = static_cast<uint32_t>(cur % kBase);
            carry = cur / kBase;
        }
        if (carry)
        {
            smaller->digits.push_back(static_cast<uint32_t>(carry));
        }
    }
    smaller->float_endpoint += delta;
    smaller->normalize();
}

BigFloat BigFloat::add_abs(const BigFloat& lhs, const BigFloat& rhs) noexcept
{
    BigFloat result;
    result.digits.clear();
    result.digits.reserve(std::max(lhs.digits.size(), rhs.digits.size()) + 1);

    uint64_t carry = 0;
    const size_t max_size = std::max(lhs.digits.size(), rhs.digits.size());
    for (size_t i = 0; i < max_size || carry; ++i)
    {
        uint64_t sum = carry;
        if (i < lhs.digits.size())
        {
            sum += lhs.digits[i];
        }
        if (i < rhs.digits.size())
        {
            sum += rhs.digits[i];
        }
        result.digits.push_back(static_cast<uint32_t>(sum % kBase));
        carry = sum / kBase;
    }

    result.float_endpoint = lhs.float_endpoint;
    result.normalize();
    return result;
}

BigFloat BigFloat::sub_abs(const BigFloat& lhs, const BigFloat& rhs) noexcept
{
    BigFloat result;
    result.digits.clear();
    result.digits.reserve(lhs.digits.size());

    int64_t borrow = 0;
    for (size_t i = 0; i < lhs.digits.size(); ++i)
    {
        int64_t diff = static_cast<int64_t>(lhs.digits[i]) - (i < rhs.digits.size() ? rhs.digits[i] : 0) - borrow;
        if (diff < 0)
        {
            diff += kBase;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }
        result.digits.push_back(static_cast<uint32_t>(diff));
    }

    result.float_endpoint = lhs.float_endpoint;
    result.normalize();
    return result;
}

BigFloat BigFloat::mul_abs(const BigFloat& lhs, const BigFloat& rhs) noexcept
{
    BigFloat result;
    result.digits.assign(lhs.digits.size() + rhs.digits.size(), 0);

    for (size_t i = 0; i < lhs.digits.size(); ++i)
    {
        uint64_t carry = 0;
        for (size_t j = 0; j < rhs.digits.size() || carry; ++j)
        {
            uint64_t cur = result.digits[i + j] + carry;
            if (j < rhs.digits.size())
            {
                cur += static_cast<uint64_t>(lhs.digits[i]) * rhs.digits[j];
            }
            result.digits[i + j] = static_cast<uint32_t>(cur % kBase);
            carry = cur / kBase;
        }
    }

    result.float_endpoint = lhs.float_endpoint + rhs.float_endpoint;
    result.negativ = false;
    result.normalize();
    return result;
}

BigFloat BigFloat::div_abs(const BigFloat& lhs, const BigFloat& rhs, int target_scale)
{
    if (E0(rhs))
    {
        throw std::runtime_error("Division by zero is not allowed");
    }

    BigFloat dividend = lhs;
    BigFloat divisor = rhs;
    align_scale(dividend, divisor);

    int extra_scale = std::max(target_scale, 0);
    if (extra_scale > 0)
    {
        int limb_shift = extra_scale / kBaseDigits;
        int digit_shift = extra_scale % kBaseDigits;
        dividend.digits.insert(dividend.digits.begin(), static_cast<size_t>(limb_shift), 0);
        dividend.float_endpoint += extra_scale;
        if (digit_shift)
        {
            uint32_t mul = pow10_small(digit_shift);
            uint64_t carry = 0;
            for (auto& limb : dividend.digits)
            {
                uint64_t cur = static_cast<uint64_t>(limb) * mul + carry;
                limb = static_cast<uint32_t>(cur % kBase);
                carry = cur / kBase;
            }
            if (carry)
            {
                dividend.digits.push_back(static_cast<uint32_t>(carry));
            }
        }
    }

    BigFloat quotient;
    quotient.digits.assign(dividend.digits.size(), 0);
    BigFloat current;
    current.digits.clear();
    current.float_endpoint = dividend.float_endpoint;

    for (int i = static_cast<int>(dividend.digits.size()) - 1; i >= 0; --i)
    {
        current.digits.insert(current.digits.begin(), dividend.digits[static_cast<size_t>(i)]);
        current.normalize();

        uint32_t left = 0, right = kBase - 1, best = 0;
        while (left <= right)
        {
            uint32_t mid = left + (right - left) / 2;
            BigFloat guess;
            guess.digits = {mid};
            guess.float_endpoint = 0;
            BigFloat prod = mul_abs(divisor, guess);
            int cmp = compare_abs(prod, current);
            if (cmp <= 0)
            {
                best = mid;
                left = mid + 1;
            }
            else
            {
                if (mid == 0)
                {
                    break;
                }
                right = mid - 1;
            }
        }

        quotient.digits[static_cast<size_t>(i)] = best;
        if (best != 0)
        {
            BigFloat subtractor;
            subtractor.digits = {best};
            subtractor.float_endpoint = 0;
            subtractor.normalize();
            current = sub_abs(current, mul_abs(divisor, subtractor));
        }
    }

    quotient.float_endpoint = dividend.float_endpoint - divisor.float_endpoint;
    quotient.normalize();
    return quotient;
}

BigFloat operator+(const BigFloat& lhs, const BigFloat& rhs)
{
    BigFloat a = lhs;
    BigFloat b = rhs;
    BigFloat::align_scale(a, b);

    if (a.negativ == b.negativ)
    {
        BigFloat result = BigFloat::add_abs(a, b);
        result.negativ = a.negativ;
        return result;
    }

    if (BigFloat::compare_abs(a, b) >= 0)
    {
        BigFloat result = BigFloat::sub_abs(a, b);
        result.negativ = a.negativ;
        return result;
    }

    BigFloat result = BigFloat::sub_abs(b, a);
    result.negativ = b.negativ;
    return result;
}

BigFloat operator+=(BigFloat& lhs, const BigFloat& rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

BigFloat operator-(const BigFloat& lhs, const BigFloat& rhs)
{
    BigFloat neg_rhs = rhs;
    neg_rhs.negativ = !rhs.negativ;
    return lhs + neg_rhs;
}

BigFloat operator-=(BigFloat& lhs, const BigFloat& rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

BigFloat operator*(const BigFloat& lhs, const BigFloat& rhs)
{
    BigFloat result = BigFloat::mul_abs(lhs, rhs);
    result.negativ = lhs.negativ != rhs.negativ;
    return result;
}

BigFloat operator*=(BigFloat& lhs, const BigFloat& rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

BigFloat operator/(const BigFloat& lhs, const BigFloat& rhs)
{
    int target_scale = lhs.precision > 0 ? lhs.precision : 18;
    BigFloat result = BigFloat::div_abs(lhs, rhs, target_scale);
    result.negativ = lhs.negativ != rhs.negativ;
    result.is_float = result.float_endpoint > 0;
    return result;
}

BigFloat operator/=(BigFloat& lhs, const BigFloat& rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

BigFloat operator%(const BigFloat& lhs, const BigFloat& rhs)
{
    BigFloat quotient = lhs / rhs;
    BigFloat product = quotient * rhs;
    BigFloat remainder = lhs - product;
    remainder.negativ = lhs.negativ;
    remainder.is_float = false;
    remainder.float_endpoint = 0;
    remainder.normalize();
    return remainder;
}

BigFloat operator%=(BigFloat& lhs, const BigFloat& rhs)
{
    lhs = lhs % rhs;
    return lhs;
}

BigFloat& BigFloat::operator++()
{
    *this = *this + BigFloat(1);
    return *this;
}

BigFloat BigFloat::operator++(int)
{
    BigFloat tmp(*this);
    ++(*this);
    return tmp;
}

BigFloat& BigFloat::operator--()
{
    *this = *this - BigFloat(1);
    return *this;
}

BigFloat BigFloat::operator--(int)
{
    BigFloat tmp(*this);
    --(*this);
    return tmp;
}

bool operator==(const BigFloat& lhs, const BigFloat& rhs)
{
    if (lhs.negativ != rhs.negativ)
    {
        return E0(lhs) && E0(rhs);
    }

    BigFloat a = lhs;
    BigFloat b = rhs;
    BigFloat::align_scale(a, b);
    return BigFloat::compare_abs(a, b) == 0;
}

bool operator!=(const BigFloat& lhs, const BigFloat& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const BigFloat& lhs, const BigFloat& rhs)
{
    if (lhs == rhs)
    {
        return false;
    }
    if (lhs.negativ != rhs.negativ)
    {
        return lhs.negativ;
    }

    BigFloat a = lhs;
    BigFloat b = rhs;
    BigFloat::align_scale(a, b);
    int cmp = BigFloat::compare_abs(a, b);
    return lhs.negativ ? (cmp > 0) : (cmp < 0);
}

bool operator>(const BigFloat& lhs, const BigFloat& rhs)
{
    return rhs < lhs;
}

bool operator<=(const BigFloat& lhs, const BigFloat& rhs)
{
    return !(lhs > rhs);
}

bool operator>=(const BigFloat& lhs, const BigFloat& rhs)
{
    return !(lhs < rhs);
}

int BigFloat::operator[](int poz) const
{
    if (poz < 0 || static_cast<size_t>(poz) >= digits.size())
    {
        throw std::out_of_range("Range Error");
    }
    return static_cast<int>(digits[static_cast<size_t>(poz)]);
}

void set_precision(BigFloat& ob1, BigFloat&, const int& n)
{
    ob1.precision = n;
}

