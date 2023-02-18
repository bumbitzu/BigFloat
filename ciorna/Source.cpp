#include <vector>
#include <string>
#include <iostream>

using namespace std;


int main() 
{
    char c = 100;

    for (int i = 0; i < 500; i++)
    {
        cout << int(c)<<" ";
        cout << c << endl;
        c++;
    }
    return 0;
}
