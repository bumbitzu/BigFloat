#pragma once
#include "BigFloat.h"
#include <string>


void assert_equals(const BigFloat& result, const std::string& expected, const std::string& test_name);
void assert_true(bool condition, const std::string& test_name);
void test_constructors();
void test_addition();
void test_subtraction();
void test_multiplication();
void test_division();
void test_factorial_stress();
