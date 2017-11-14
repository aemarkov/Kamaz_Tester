// KamazTester.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <Kamaz\Algorithms.h>

using namespace std;

int main()
{
	A1();

	for (auto act : getActions())
		cout << act << "\n";

    return 0;
}

