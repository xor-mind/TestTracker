# TestTracker
A test harness written on top of Boost Test

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
