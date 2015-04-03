/*
// Why this is useful to a c++ programmer /////////////////////////////////////
A module built on top of Boost Test that adds some details to boosts tests, 
i.e. The current test #, how many tests are left, and a contextual name for the test; 
all of which can aid in understanding the testing.

Normally when using boost test you get the following ouput:
Running 3 test cases...

*** No errors detected.

As you can see, there's no information on what was tested. By correctly using the 
TestTracker module, you would instead get this output:

Running 3 test cases...

Test: #1 Push Button ***PASSED!!*

Test: #2 Changing Wnd Class
		 Brush Change
		 Cursor Chang
***PASSED!!*

Test: #3 SetClassName before WndInit ***PASSED!!*

						*************************
							**success no errors**
							**		end of tests*
							**Hit enter to stop**
					    *************************
*** No errors detected.

If there's an error during the test, you will still get the same informative 
error line # and source file name, just as if you were using boost test 
by itself.

// How to use this in your own code ///////////////////////////////////////////
To get the most out of this it's expected you to know how to use Boost Test.
Instead of defining a BOOST_AUTO_TEST_CASE(testName) or a TT_FIXTURE_TEST_CASE, you implement your test
code in a CreateUnitTest code block, i.e.
CreateUnitTest(testName, "Test Name")
{
	BOOST_REQUIRE(true);
}

That's it. You're test will now be numbered based on it's order in relation to the other tests,
and your test will have a string identifier,
which the test tracker will use to let you know it's testing this code.

// Implementation details /////////////////////////////////////////////////////
The module's most important components are the class TestTracker 
and the macro CreateUnitTest.

All tests are basically TestTracker classes. By having all tests be test tracker instances,
it's easy to know how many tests there are, as well as the name & number of the test.

Instead of having the user create a new child of TestTracker, create an instance, and fill
in the user defined data, there's a macro named CreateUnitTest, that does all of this while
hiding the copypasta which would normally be written for each test.
*/

#ifndef TEST_TRACKER_H
#define TEST_TRACKER_H


#include "Console.h"
#include "adams_macros.h"
#include <iostream>
#include <Windows.h>
#include <tchar.h>

struct TestTracker
{
	enum Style { minimal, verbose };
	Style style;

	TestTracker(TCHAR* const testName, Style s = defaultStyle() )
		: style(s)
	{
		SetName(testName);
		myTestNumber = currentTest()++;
		if (active())
		{
			if (myTestNumber == 1)
				BeginTests();
			if ( style == verbose ) {
				hr();
				say "Test: #" << myTestNumber newl
			}
			else
				say "\nTest: #" << myTestNumber << " " << Name() << " ";
				
		}
	}

	~TestTracker()
	{
		if (active())
		{
			if ( myTestNumber == totalTests() ) {
				if ( style == verbose ) {
					hr();
				}
				else {
					endline;
				}
				EndOfTests();
			}
		}
		if ( myTestNumber == totalTests() ) 
			wait
	}

	void RunTest()
	{
		//if (myTestNumber == -1)
		//	return;
		if (active()) 
		{
			if (style == verbose) 
			{
				tMinusTesting(Name(), tMinusTime()); 
				SayTesting(Name());
			}
		}
		Test();
		if (active())  {
			if (style == verbose) {
				SayPassed(Name()); 
				endline
			}
			else {
				say "***PASSED!!*" newl
			}
		}
	}

	virtual void Test() = 0;

	void BeginTests()
	{
		if (active())
		{
			HWND hWnd = GetConsoleWindow();
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			CenterConsoleWindow();
			if ( style == verbose )
			{
				say "Hit enter to begin the tests." newl
				wait
			}
		}
	}

	void EndOfTests()
	{
		if (active())
		{
			CenteredConsoleOutput("*************************");
			CenteredConsoleOutput("**success no errors**");
			CenteredConsoleOutput("================");
			CenteredConsoleOutput("|*End of tests*|");
			CenteredConsoleOutput("================");
			CenteredConsoleOutput("**Hit enter to stop**");
			CenteredConsoleOutput("*************************");
		}
	}

	void PauseBeforeTesting(TCHAR* const testName) {
		if (!active()) return;
		tsay "you have requested a pause right before the start of testing: " << testName stop; 
		chill
	}
	void SayTesting(TCHAR* const testName) {
		if (!active()) return;
		tsay "Now Testing: " << testName; 
		Quice(say "."; Sleep(50)); endline
	}
	void SayPassed(TCHAR* const testName) {
		if (!active()) return;
		tsay "** PASSED *** testing " << testName; 
		Thrice(say "."; Sleep(50)); endline
	}
	void tMinusTesting(TCHAR* const testName, UINT seconds )
	{
		if (!active()) return;
		tsay _T("Begin Testing ") << testName << " in:" stop;
		for ( UINT i = 0; i < seconds; ++i ) {
			say seconds - i;
			Twice(Sleep(50); say "."); Sleep(50);
		}
		endline endline;
	}

	static int& totalTests() { static int tt = 0; return tt; }
	static int& currentTest() { static int ct = 1; return ct; }
	static bool& active() { static bool a = true; return a; }
	static void RegisterTest() { totalTests() += 1; }
	static int tMinusTime() { static int s = 3; return s; }
	struct AddTest { AddTest() { TestTracker::RegisterTest(); } };
	TCHAR* Name() { return testName; }
	void SetName(TCHAR* testName) { _tcscpy_s(this->testName, 256, testName); }
	static Style& defaultStyle() { static Style s = TestTracker::minimal; return s; }
private:
	
	int myTestNumber;
	TCHAR testName[256];
};

//#define RegisterTestClass(x) TestTracker::AddTest addTest##x; \
//struct x; \
//BOOST_FIXTURE_TEST_CASE(run##x, x) {}				          \	
//struct x : public TestTracker								  \

#define TT_FIXTURE_TEST_CASE(f) TestTracker::AddTest addTest##f; \
BOOST_FIXTURE_TEST_CASE(run##f, f)							     \

#define TEST_NAME(x) static TCHAR* TestName() { static TCHAR* s = _T(x); return s; }

#define		TT						TestTracker						
#define		CreateFixture(x)		TT_FIXTURE_TEST_CASE(x)

#define CreateUnitTest( TestClassName, TestStringName )					\
																		\
struct TestClassName : TT 										        \
{																		\
	TEST_NAME( TestStringName );                                        \
																		\
	TestClassName() : TT(TestName()) { RunTest(); }						\
	void Test();														\
};																		\
CreateFixture(TestClassName) { }										\
void TestClassName::Test()

#endif