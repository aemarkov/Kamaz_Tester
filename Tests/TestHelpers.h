#pragma once

#include <string>
#include <vector>
#include "CppUnitTest.h"
#include <sstream>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void AssertVectors(vector<string>& expected, vector<string>& real);

