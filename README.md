# BigFloat Class<br>
The BigFloat class is a C++ class that provides a high-precision floating-point arithmetic library. The class is designed to be easy to use and provides a set of operators and functions that allow for easy manipulation of large numbers with high precision. The class provides basic arithmetic operations, such as addition, subtraction, multiplication, division, and modulus, as well as comparison operators and prefix/postfix increment and decrement operators.<br>

## Public Members
### Constructors
* BigFloat(): Default constructor that initializes a new BigFloat object to 0.
* BigFloat(long long): Constructor that initializes a new BigFloat object from a long long integer.
* BigFloat(const std::string&): Constructor that initializes a new BigFloat object from a string.
* BigFloat(const BigFloat&): Copy constructor that initializes a new BigFloat object from an existing BigFloat object.
### Operators
* BigFloat& operator = (const BigFloat&): Assignment operator that assigns one BigFloat object to another.
* friend std::istream& operator >> (std::istream&, BigFloat&): Input stream operator that reads a BigFloat object from an input stream.
* friend std::ostream& operator << (std::ostream&, const BigFloat&): Output stream operator that writes a BigFloat object to an output stream.
* friend BigFloat operator + (const BigFloat&, const BigFloat&): Addition operator that adds two BigFloat objects together.
* friend BigFloat operator += (BigFloat&, const BigFloat&): Compound assignment operator for addition that adds a BigFloat object to another BigFloat object and stores the result in the first object.
* friend BigFloat operator - (const BigFloat&, const BigFloat&): Subtraction operator that subtracts one BigFloat object from another.
* friend BigFloat operator -= (BigFloat&, const BigFloat&): Compound assignment operator for subtraction that subtracts a BigFloat object from another BigFloat object and stores the result in the first object.
* friend BigFloat operator * (const BigFloat&, const BigFloat&): Multiplication operator that multiplies two BigFloat objects together.
* friend BigFloat operator *= (BigFloat&, const BigFloat&): Compound assignment operator for multiplication that multiplies a BigFloat object by another BigFloat object and stores the result in the first object.
* friend BigFloat operator / (const BigFloat&, const BigFloat&): Division operator that divides one BigFloat object by another.
* friend BigFloat operator /= (BigFloat&, const BigFloat&): Compound assignment operator for division that divides a BigFloat object by another BigFloat object and stores the result in the first object.
* friend BigFloat operator % (const BigFloat&, const BigFloat&): Modulus operator that returns the remainder.
* friend BigFloat operator %= (BigFloat&, const BigFloat&): Compound modulus operator for division that divides a BigFloat object by another BigFloat object and stores the remainder in the first object.
* BigFloat& operator ++ (): Prefix increment operator that overloads the prefix ++ operator for the BigFloat class and returns a reference to the modified object.
* BigFloat operator ++ (int fictiv): Postfix increment operator that overloads the postfix ++ operator for the BigFloat class and returns a copy of the object before it was incremented.
* BigFloat& operator -- (): Prefix decrement operator that overloads the prefix -- operator for the
