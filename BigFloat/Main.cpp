#include "BigFloat.h"
#include <iostream>
using namespace std;
int main()
{
	try
	{
		//Finding pi
		BigFloat circumference("1000000"), diameter("3183099"),pi;

		// set prec
		set_precision(circumference, diameter,2000);
		
		pi = circumference / diameter;
		
		cout << char(-29) << " " << char(-9) << " ";
		cout << pi;
	}
	catch (const char* c)
	{
		cout << c;
	}
	
}
