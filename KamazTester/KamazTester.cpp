// KamazTester.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <Kamaz\Algorithms.h>

using namespace std;

int main()
{
	ResetEnv();
	circuitState.AddInput(0);

	for(int i = 0; i < 5 * 60 + 2; i++)
		contour4Pressure.AddInput(5);

	budPressure.AddInput(2);
	budPressure.AddInput(2);
	budPressure.AddInput(2);
	budPressure.AddInput(1);

	A1();

	for (auto act : getActions())
		cout << act << "\n";

    return 0;
}

