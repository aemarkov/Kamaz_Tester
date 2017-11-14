#include "stdafx.h"
#include "CppUnitTest.h"
#include <Kamaz/Algorithms.h>
#include "TestHelpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace Tests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(A1_OK)
		{
			vector<string> expected_actions;
			expected_actions.push_back(SEND_CONTOUR_4_REQUEST);
			expected_actions.push_back(GET_CONTOUR_4_RESPONSE);
			expected_actions.push_back(valvesToString(getBoolMask({false, false, false, false, false, false, false, true, false, false})));
			expected_actions.push_back(DELAY + 1000);


			ResetEnv();

			contour4Pressure.AddInput(5);
			budPressure.AddInput(2);
			budPressure.AddInput(2);
			budPressure.AddInput(2);
			budPressure.AddInput(1);

			A1();
			
			AssertVectors(expected_actions, getActions());
		}

	};
}