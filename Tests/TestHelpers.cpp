#include "TestHelpers.h"

bool CompareVectors(vector<string>& expected, vector<string>& real);
wstring PrintVectors(vector<string>& expected, vector<string>& real);
void PrintVector(wstringstream& s, vector<string>& v);


void AssertVectors(vector<string>& expected, vector<string>& real)
{
	bool equal = CompareVectors(expected, real);
	wstring message = PrintVectors(expected, real);
	Assert::IsTrue(equal, message.c_str());
}

bool CompareVectors(vector<string>& expected, vector<string>& real)
{
	if (expected.size() != real.size())
		return false;

	for (int i = 0; i < expected.size(); i++)
		if (expected[i] != real[i])
			return false;

	return true;
}

wstring PrintVectors(vector<string>& expected, vector<string>& real)
{
	wstringstream s;

	s << "\n";

	s << "Real:\n";
	PrintVector(s, real);
	s << "\nExpected:\n";
	PrintVector(s, expected);

	return s.str();
}

void PrintVector(wstringstream& s, vector<string>& v)
{
	for (int i = 0; i < v.size(); i++)
		s << wstring(v[i].begin(), v[i].end()) << "\n";
}