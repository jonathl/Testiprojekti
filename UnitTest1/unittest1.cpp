#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Testiprojekti/Main.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(test1)
		{
			UVector u = m_genRandomUnitVector();
			Assert::AreEqual(1.0f, round((u.x*u.x) + (u.y*u.y)));
		}

	};
}