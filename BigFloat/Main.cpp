#include "BigFloat.h"
#include <iostream>
using namespace std;
int main()
{
	try
	{
		//Finding pi
		BigFloat circumference("100000"), diameter("31830.99"),pi;
		set_precision(circumference, diameter,1000);
		pi = circumference / diameter;
		cout << char(-29) << " " << char(-9) << " ";
		cout << pi;
		
	}
	catch (const char* c)
	{
		cout << c;
	}
	
}